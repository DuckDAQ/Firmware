/************************************************************************************//**
* \file     core.h
* \brief    This module implements core DAQ functionality.
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <asf.h>
#include "core.h"


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/* Structure with all the settings. */
static daq_settings_t settings;

/* PDC(peripheral DMA controller) data packet for transfer. */      //Nastavimo mu start in size
pdc_packet_t adc_pdc1, adc_pdc2;

/* Pdc hardware registers */
Pdc *adc_pdc_pntr;

/* to store data??? */
uint16_t adc_raw_data1[ADC_RAW_DATA_SIZE];
uint16_t adc_raw_data2[ADC_RAW_DATA_SIZE];
uint32_t adc_raw_accumulator[ADC_RAW_DATA_SIZE];
volatile uint32_t rep_cntr; //repetition counter
volatile uint32_t new_data = 0;
volatile uint32_t acqusition_in_progress = 0;
volatile uint32_t data_bank = 0;
volatile uint32_t avg_cntr;
uint32_t raw_data_size;
uint32_t nb_enables_ch;


/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
#if ADC_CORE_DEBUG == 1
	void pio_init (void)
	{
		pmc_enable_periph_clk(ID_PIOA);
		pio_set_output(PIOA, ADC_DEBUG_PIN, LOW, DISABLE, DISABLE); //indicator pin for ADC
		pio_set_output(PIOA, TIMER_DEBUG_PIN, LOW, DISABLE, DISABLE);//indicator pin for Timer0
	} /*** end of pio_init ***/
#endif //ADC_CORE_DEBUG == 1


/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
void core_init (void)
{
	/* Init ADC */
	pmc_enable_periph_clk(ID_ADC);
	adc_init(ADC, sysclk_get_cpu_hz(), ADC_CLK, 0);
	adc_configure_timing(ADC, 15, ADC_SETTLING_TIME_0, 0);
	adc_configure_trigger(ADC, ADC_TRIG_SW, 0); //! WARNING! Bug in ASF! ADC_MR_FREERUN_ON doesn't actually enables freerun mode!
	//adc_check(ADC, sysclk_get_cpu_hz());
	ADC->ADC_COR |= (ADC_COR_DIFF0 | ADC_COR_DIFF1 | ADC_COR_DIFF2 | ADC_COR_DIFF3 |
                   ADC_COR_DIFF4 | ADC_COR_DIFF5 | ADC_COR_DIFF6 | ADC_COR_DIFF7); // set channels to differential
	
  //ADC->ADC_CGR = 0x00005555; // set gain to 1
	//TODO: make this configurable from settings->adcGain
	//TODO: call core_init before core_configure, so you can set adcGain
	//If adcGain has not been set, use default value.
	int gain = 0x11;
	//int gain = settings->adcGain;
	ADC -> ADC_CGR = ( ADC_CGR_GAIN0(gain) | ADC_CGR_GAIN1(gain) | ADC_CGR_GAIN2(gain) | ADC_CGR_GAIN3(gain));
	adc_set_bias_current(ADC, 1);
  
	#if ADC_CORE_DEBUG == 1
		pio_init();
	#endif //ADC_CORE_DEBUG == 1

	adc_pdc_pntr = adc_get_pdc_base(ADC); // init DMA

	//Both pdc1 & pdc2:
	adc_pdc1.ul_addr = adc_raw_data1;   //	!Warning		assignment makes integer from pointer without a cast [-Wint-conversion]
	adc_pdc2.ul_addr = adc_raw_data2;   //	!Warning		assignment makes integer from pointer without a cast [-Wint-conversion]

	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_SetPriority(ADC_IRQn, ADC_IRQ_PRIORITY);
	NVIC_EnableIRQ(ADC_IRQn);

	//init timer 0
	pmc_enable_periph_clk(ID_TC0); //Enable a peripheral's clock
	tc_init(TC0, TIMER_CH, TC_CMR_TCCLKS_TIMER_CLOCK4 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC ); //Configure TC for timer, waveform generation, or capture.
	tc_write_rc(TC0, TIMER_CH, 50000); // Write to TC Register C (RC) on the selected channel.
	tc_enable_interrupt(TC0, TIMER_CH, TC_IER_CPCS); //Enable the TC interrupts on the specified channel.
	NVIC_SetPriority(TC0_IRQn, TIMER_IRQ_PRIORITY); //Set Priority Grouping
	NVIC_EnableIRQ(TC0_IRQn); //Enable External Interrupt
  
  //DAC INIT
	//sysclk_enable_peripheral_clock(ID_DACC); // enable clock for DACC
	pmc_enable_periph_clk(ID_DACC); // enable clock for DACC
	
	//begin DACC configuration by resetting the DACC hardware
	/* Reset DACC registers */
	dacc_reset(DACC);
	
	//write one 16-bit value at a time, not two 16-bit values in one 32-bit word
	/* Half word transfer mode */
	dacc_set_transfer_mode(DACC, 0); 
	
	/* Power save:
	 * sleep mode  - 0 (disabled)
	 * fast wakeup - 0 (disabled) */
	dacc_set_power_save(DACC, 0, 0);
	
	//select channel 0 and 1, #define in core.h
	/* Disable TAG and select output channel DACC_CHANNEL */
	dacc_set_channel_selection(DACC, DACC_CHANNEL0);
	dacc_set_channel_selection(DACC, DACC_CHANNEL1);
	
	/* Timing:
        * refresh        - 0x08 (1024*8 dacc clocks)
        * max speed mode -    0 (disabled)
        * startup time   - 0x10 (1024 dacc clocks)
		
		
		p_dacc	Pointer to a DACC instance.
		ul_refresh	Refresh period setting value.
		ul_maxs	Max speed mode configuration.
		ul_startup	Startup time selection.
		*/
	
  //dacc_set_timing(DACC, 0x08, 0, 0x10);
	
	//dacc_enable(DACC);
	dacc_enable_channel(DACC, DACC_CHANNEL0);
	dacc_enable_channel(DACC, DACC_CHANNEL1);	
	//dacc_analog_control defined in core.h
	dacc_set_analog_control(DACC, DACC_ANALOG_CONTROL);
} /*** end of core_init ***/


/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
void timer_set_compare_time (uint32_t tim)
{
	if(tim > 50000) tim = 50000;
	tc_write_rc(TC0, TIMER_CH, tim);
} /*** end of timer_set_compare_time ***/


/*
TODO:
-nastavi periodo vzorcenja
settings.acqusitionTime

-nastavi st vzorcev za povprecenje
settings.averaging

-nastavi st zaporednih meritev (0=neskoncno, >0 = koncno stevilo meritev)
settings.acquisitionNbr

-nastavi zaporedje kanalov (0=brez, 1-4 = kanal)
settings.sequence[i]

*/

/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
bool core_configure (daq_settings_t * master_settings)
{
  /* Make sure pointer is valid */
  if(master_settings != NULL)
  {
    /* first copy settings globally */
    settings = *master_settings;

	  /* Clear averaging accumulator */
	  core_clear_avg_acuum();

	  /* Disable all ADC channels */
	  adc_disable_all_channel(ADC);
  
	  /* Enable ADC channels that are enabled in settings struct */
    uint8_t n;
	  for(n = 0; n < 4; n++)
	  {
		  if(settings.sequence[n] > 0)
		  {
			  nb_enables_ch++;
			  adc_enable_channel(ADC, settings.sequence[n]);
		  }
	  }

	  //configure DMA (treba nastavit start & size)
	  //raw_data_size = settings.averaging * nb_enables_ch;
	  adc_pdc1.ul_size = nb_enables_ch;
	  adc_pdc2.ul_size = nb_enables_ch;

	  pdc_rx_init(adc_pdc_pntr, &adc_pdc1, &adc_pdc2);
	  data_bank = 0;
	  adc_enable_interrupt(ADC, ADC_IER_ENDRX);

	  /* set average counter */
	  avg_cntr = settings.averaging;
    
	  /* Set timer for acquisition time */
	  timer_set_compare_time(US_TO_TC(settings.acqusitionTime));
    
	  #if ADC_CORE_DEBUG == 1
			  TIMER_DEBUG_PIN_CLR;
	  #endif //ADC_CORE_DEBUG == 1
    
    //DAC 
    //if(settings->DACval[0]!=0 || settings->DACval[1]!=0)
    //{
      
      //dacc_write_conversion_data(DACC, settings->DACval[0]);
      //Reset?
      //settings->DACval[0]=-1;
      //settings->DACval[1]=;
      //
    //}
    //DacSetVal(0, settings->DACval[0]);
    
    return TRUE;
  }
  return FALSE;
} /*** end of core_configure ***/


/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
void core_start (void)
{
	pdc_enable_transfer(adc_pdc_pntr, PERIPH_PTCR_RXTEN);
	acqusition_in_progress = 1;
	tc_start(TC0, TIMER_CH);
	//ADC->ADC_MR |= ADC_MR_FREERUN; //due to a bug in ASF we enable freerun mode manualy
	adc_start(ADC);
} /*** end of core_start ***/


/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
void DacSetVal(daq_settings_t *master_settings)
{	
	int status = dacc_get_interrupt_status(DACC);

	if ((status & DACC_ISR_TXRDY) == DACC_ISR_TXRDY && master_settings->DACflag==1)
	{
		master_settings->DACflag==0;
		//loop for both channels
		for(int i=0;i<2;i++){
			dacc_set_channel_selection(DACC, i); //first channel 0, then channel 1			
			dacc_write_conversion_data(DACC, master_settings->DACval[i]);
		}
	}
} /*** end of DacSetVal ***/


/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
core_status_t core_status_get (void)
{
	if(acqusition_in_progress) return CORE_RUNNING;
	else return CORE_STOPED;
} /*** end of core_status_get ***/


/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
void core_clear_avg_acuum (void)
{
	uint32_t n;
	//clear averaging accumulator
	for(n = 0; n < ADC_RAW_DATA_SIZE; n++)
	{
		adc_raw_accumulator[n] = 0;
	}
} /*** end of core_clear_avg_acuum ***/


/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
void ADC_Handler (void)
{
	uint32_t n;
	if(adc_get_status(ADC) & ADC_ISR_ENDRX) // this gets triggered when acquisition of all samples for one averaging is complete

	{
		#if ADC_CORE_DEBUG == 1
		ADC_DEBUG_PIN_SET;
		#endif //ADC_CORE_DEBUG == 1

		//ADC->ADC_MR &= (~ADC_MR_FREERUN); //stop adc
		if(!data_bank) // new data resides in adc_raw_data1
		{
			pdc_rx_init(adc_pdc_pntr, NULL, &adc_pdc2);
			pdc_enable_transfer(adc_pdc_pntr, PERIPH_PTCR_RXTEN);
			adc_start(ADC);

			data_bank = 1;
			for(n = 0; n < 4; n++)
			{
				adc_raw_accumulator[n] += adc_raw_data1[n];
			}
		}
		else // new data resides in adc_raw_data2
		{
			pdc_rx_init(adc_pdc_pntr, NULL, &adc_pdc1);
			pdc_enable_transfer(adc_pdc_pntr, PERIPH_PTCR_RXTEN);
			adc_start(ADC);
			data_bank = 0;

			for(n = 0; n < ADC_RAW_DATA_SIZE; n++)
			{
				adc_raw_accumulator[n] += adc_raw_data2[n];
			}

		}
		#if ADC_CORE_DEBUG == 1
		ADC_DEBUG_PIN_CLR;
		#endif //ADC_CORE_DEBUG == 1
		if(!(--avg_cntr))
		{
			pdc_disable_transfer(adc_pdc_pntr, PERIPH_PTCR_RXTEN);
			//do this to clear DMA flag
			pdc_rx_init(adc_pdc_pntr, &adc_pdc1, NULL);
			//report new data
			new_data = 1;
			avg_cntr = settings.acquisitionNbr;
		}


	}
} /*** end of ADC_Handler ***/


/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
void TC0_Handler (void) //?Timer/Counter 0 handler, se uporablja za ADC??
{
	if((tc_get_status(TC0, 0) & TC_SR_CPCS))
	{
		#if ADC_CORE_DEBUG == 1
			TIMER_DEBUG_PIN_TGL;
		#endif //ADC_CORE_DEBUG == 1
		if(--rep_cntr)
		{
			pdc_rx_init(adc_pdc_pntr, &adc_pdc1, &adc_pdc2);
			data_bank = 0;
			pdc_enable_transfer(adc_pdc_pntr, PERIPH_PTCR_RXTEN);
			adc_start(ADC);
		}
		else
		{
			acqusition_in_progress = 0;
			tc_stop(TC0, TIMER_CH);
		}
	}
} /*** end of TC0_Handler ***/

/************************************ end of core.c ***********************************/