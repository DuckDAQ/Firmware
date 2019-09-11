/************************************************************************************//**
* \file     core.h
* \brief    This module implements core DAQ functionality.
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <asf.h>
#include "core.h"


/***************************************************************************************
* Function prototypes
****************************************************************************************/
bool timerInit(void);
bool dacInit(void);
bool adcInit(void);
bool pdcInit(void);
void comTxEmptyCallback(uint8_t port);

/****************************************************************************************
* Local data declarations
****************************************************************************************/
/* Structure with all the settings. */
static daq_settings_t * settings;
/* ADC PDC variables. */
Pdc * adcPdc;
pdc_packet_t adcPdcPacket;
uint16_t adcPdcBuff[ADC_BUFFER_SIZE][4];
/* DAC PDC variables. */
Pdc * daccPdc;
pdc_packet_t daccPdcPacket;
/* Indicates PDC transfer was complete. */
bool pdcAdcTransfetComplete = 0;
/* Sync bytes for BIN mode. */


/****************************************************************************************
*                             C O R E   U T I L I T I E S
****************************************************************************************/
/************************************************************************************//**
** \brief
** \param
** \return
**
****************************************************************************************/
bool coreConfigure (daq_settings_t * master_settings)
{
  /* Make sure pointer is valid */
  if(master_settings != NULL)
  {
    /* first copy settings pointer globally. */
    settings = master_settings;
    /* Initialize block size pointer. */
    uint32_t tmp = (uint32_t)master_settings->blockSize;
    master_settings->blockSize = &adcPdcPacket.ul_size;
    *master_settings->blockSize = tmp;
    /* Return true */
    return TRUE;
  }
  /* Return false */
  return FALSE;
} /*** end of core_configure ***/


/************************************************************************************//**
** \brief
**
****************************************************************************************/
bool coreStart(void)
{  
  /* Check if core configure was called. */
  if(settings != NULL)
  {
			/* Initialize timer 0. */
			if(!timerInit())
			{
				return false;
			}
  
			/* Initialize PDC */
			if(!pdcInit())
			{
					return false;
			}
  
			/* Initialize ADC */
			if(!adcInit())
			{
				return false;
			}
			/* Initialize DAC */
			if(!dacInit())
			{
				return false;
			}
  }  
  /* Return result. */
  return true;
} /*** end of coreInit ***/


/****************************************************************************************
*                               A D C   U T I L I T I E S
****************************************************************************************/
/************************************************************************************//**
** \brief     Initialize ADC with some basic settings.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool adcInit(void)
{
    /* Enable the specified peripheral clock. */
    if(pmc_enable_periph_clk(ID_ADC))
    {
      return false;
    }
  
    /* Initialize the given ADC with the specified ADC clock and startup time. */
    if(adc_init(ADC, sysclk_get_cpu_hz(), ADC_CLK, 0))
    {
      return false;
    }
  
    /* Configure ADC timing */
    adc_configure_timing(ADC, 15, ADC_SETTLING_TIME_0, 0);
    /* Configure the conversion resolution. */
    adc_set_resolution(ADC, ADC_MR_LOWRES_BITS_12);
    /* Disable all ADC channels. */
    adc_disable_all_channel(ADC);
    /* Enable bias current. */
    adc_set_bias_current(ADC, 1);
    /* Set ADC trigger to TIO Output of the Timer Counter Channel 0. */
    adc_configure_trigger(ADC, ADC_TRIG_TIO_CH_0, 0);
    /* Set all channels in differential mode */
    ADC->ADC_COR |= ( ADC_COR_DIFF0 | ADC_COR_DIFF1 | ADC_COR_DIFF2 | ADC_COR_DIFF3 |
                      ADC_COR_DIFF4 | ADC_COR_DIFF5 | ADC_COR_DIFF6 | ADC_COR_DIFF7 );
    /* Set gain to 1 for all channels. */
    ADC->ADC_CGR = ( (((ADC_GAIN_1 << 2) | ADC_GAIN_1) <<  4) |
                     (((ADC_GAIN_1 << 2) | ADC_GAIN_1) <<  0) |
                     (((ADC_GAIN_1 << 2) | ADC_GAIN_1) << 12) |
                     (((ADC_GAIN_1 << 2) | ADC_GAIN_1) <<  8) );
    /* Enable channel number in measurement's upper 4 bits. */
    ADC->ADC_EMR |= ADC_EMR_TAG;
    /* Allows different analog settings for each channel. */
    ADC->ADC_MR |= ADC_MR_ANACH;
    /* Enable channels. */
    adcSetChannels();
    adcHandler(true);
  
  /* Return result. */
  return true;
}

/************************************************************************************//**
** \brief     Enables ADC channels and sets sequencer.
**
****************************************************************************************/
void adcSetChannels(void)
{
  uint8_t idx = 0;
  /* DAQ channel to ADC channel map */
  static uint8_t ch2ch[5];
  ch2ch[0] = 0;
  ch2ch[1] = DAQ_CH_1_ADC_CH;
  ch2ch[2] = DAQ_CH_2_ADC_CH;
  ch2ch[3] = DAQ_CH_3_ADC_CH;
  ch2ch[4] = DAQ_CH_4_ADC_CH;
  
  /* Disable all ADC channels. */
  adc_disable_all_channel(ADC);
  
  /* Loop through all the channels. */
  for(idx = 0; ((idx < 4) && settings->sequence[idx]); idx++)
  {
    /* Enable the specified ADC channel. */
    adc_enable_channel(ADC, ch2ch[settings->sequence[idx]]);
  }
  
}


/************************************************************************************//**
** \brief
**
****************************************************************************************/
void adcSetGain(void)
{
  ADC->ADC_CGR = ( (((settings->ADCgain[1] << 2) | settings->ADCgain[1]) << 12) |
                   (((settings->ADCgain[0] << 2) | settings->ADCgain[0]) <<  8) |
                   (((settings->ADCgain[3] << 2) | settings->ADCgain[3]) <<  4) |
                   (((settings->ADCgain[2] << 2) | settings->ADCgain[2]) <<  0) );
} /*** end of DacSetVal ***/


/************************************************************************************//**
** \brief
**
****************************************************************************************/
void adcSetRes(void)
{
  if(settings->ADClowRes) ADC->ADC_MR |= ADC_MR_LOWRES;
  else                    ADC->ADC_MR &= ADC_MR_LOWRES;
} /*** end of DacSetVal ***/


/************************************************************************************//**
** \brief     Enable or disable ADC interrupt handler
** \param     ADC interrupt handler state
**
****************************************************************************************/
void adcHandler(bool state)
{
  if(state)
  {
    /* Enable the ADC interrupts */
    adc_enable_interrupt(ADC, ADC_IER_RXBUFF);
    adc_enable_interrupt(ADC, ADC_IER_ENDRX);
    /* Set Interrupt Priority */
    NVIC_SetPriority(ADC_IRQn, ADC_IRQ_PRIORITY);
    /* Enable External Interrupt */
    NVIC_EnableIRQ(ADC_IRQn);
  }
  else
  {
    /* Disable the ADC interrupts */
    adc_disable_interrupt(ADC, ADC_IER_RXBUFF);
    adc_disable_interrupt(ADC, ADC_IER_ENDRX);
  }
}
void dacHandler(bool state)
{
	  if(state)
  {
    /* Enable the DACC interrupts */
    dacc_enable_interrupt(DACC, DACC_IMR_ENDTX);
    /* Set Interrupt Priority */
    NVIC_SetPriority(DACC_IRQn, DACC_IRQ_PRIORITY);
    /* Enable External Interrupt */
    NVIC_EnableIRQ(DACC_IRQn);
  }
  else
  {
    dacc_disable_interrupt(DACC, DACC_IMR_ENDTX);
  }
}

/************************************************************************************//**
** \brief     Analog to digital converter interrupt service routine
**
****************************************************************************************/
void ADC_Handler(void)
{
  uint32_t status = adc_get_status(ADC);
  
  if((status & ADC_ISR_RXBUFF) == ADC_ISR_RXBUFF)
  {
    if(!pdcAdcTransfetComplete)
    {
      pdc_rx_init(adcPdc, &adcPdcPacket, NULL);
      pdc_enable_transfer(adcPdc, PERIPH_PTCR_RXTEN);
    }
    else
    {
      timerStop();
      adcHandler(false);
      pdcAdcTransfetComplete = false;
      settings->acqusitionTime = 1000000;
      timerSetTimePeriod();
      settings->com->len = sprintf((char*)settings->com->buf, "Sample rate to high!\n\r");
      settings->com->printBuf(settings->com->buf, settings->com->len);
    }      
  }
  
  if((status & ADC_ISR_ENDRX) == ADC_ISR_ENDRX)
  {
    pdcAdcTransfetComplete = true;
    settings->com->printBuf(settings->syncBytes, 2);
  }
    
}


/****************************************************************************************
*                               D A C   U T I L I T I E S
****************************************************************************************/
/************************************************************************************//**
** \brief     Initialize DAC
**
****************************************************************************************/
bool dacInit(void)
{
  pmc_enable_periph_clk(ID_DACC); // enable clock for DACC
  
  //begin DACC configuration by resetting the DACC hardware
  /* Reset DACC registers */
  dacc_reset(DACC);
  
  //write one 16-bit value at a time, not two 16-bit values in one 32-bit word
  /* Half word transfer mode */
  dacc_set_transfer_mode(DACC,	1); //0=HalfMode
  
  /* Power save:
   * sleep mode  - 0 (disabled)
   * fast wakeup - 0 (disabled) */
  //dacc_set_power_save(DACC, 0, 0);
  
		/*
		 * \param p_dacc Pointer to a DACC instance.
		 * \param ul_refresh Refresh period setting value.
		 * \param ul_maxs Max speed mode configuration.
		 * \param ul_startup Startup time selection.
		*/
		dacc_set_timing(DACC, 0x08, 0, 0x10);
		
		//dacc_set_channel_selection(DACC, DACC_CHANNEL0);
  dacc_enable_flexible_selection(DACC);
  /* Timing:
        * refresh        - 0x08 (1024*8 dacc clocks)
        * max speed mode -    0 (disabled)
        * startup time   - 0x10 (1024 dacc clocks)
    
    
    p_dacc  Pointer to a DACC instance.
    ul_refresh  Refresh period setting value.
    ul_maxs Max speed mode configuration.
    ul_startup  Startup time selection.
    */
		
	/*Choose the TIO output from timer/counter channel 1 as the trigger.
	Note that "channel 1" in this case refers only to channel 1 of timer/counter
	module 0, not channel 1 of timer/counter module 1. Also, the datasheet
	specifies only "TIO output" as the trigger source, without clarifying whether
	this refers to TIOA or TIOB or both. With a little experimentation I determined
	that only TIOA will trigger the DAC, not TIOB.*/
  dacc_set_trigger(DACC, 2);
  
  //dacc_enable(DACC);
  dacc_enable_channel(DACC, DACC_CHANNEL0);
		
				//dacc_set_channel_selection(DACC, DACC_CHANNEL1);
  //dacc_set_trigger(DACC, 2);
  dacc_enable_channel(DACC, DACC_CHANNEL1);
		
		tc_start(TC0, 1);
		dacc_enable_interrupt(DACC, DACC_IMR_ENDTX);
		NVIC_EnableIRQ(DACC_IRQn);
		pdc_enable_transfer(daccPdc, PERIPH_PTCR_TXTEN);
  //dacc_analog_control defined in core.h
  //dacc_set_analog_control(DACC, DACC_ANALOG_CONTROL);
  
  return true;
}


/************************************************************************************//**
** \brief     Updates DAC output value.
**
****************************************************************************************/
void coreSetDacVal(void)
{
  uint16_t status = dacc_get_interrupt_status(DACC);
  
  if ((status & DACC_ISR_TXRDY) == DACC_ISR_TXRDY)
  {
    //loop for both channels
    for(uint8_t i = 0; i < 2; i++){
      dacc_set_channel_selection(DACC, i); //first channel 0, then channel 1      
      dacc_write_conversion_data(DACC, settings->DACval[i]);
    }
  }
} /*** end of DacSetVal ***/


/****************************************************************************************
*                               P D C   U T I L I T I E S
****************************************************************************************/
/************************************************************************************//**
** \brief     Initialize PDC controller
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool pdcInit(void)
{
  /* Get PDC registers base address. */
  adcPdc = adc_get_pdc_base(ADC);
  /* Only continue if pointer is valid */
  if(adcPdc != NULL)
  {
    /* Initialize PDC packet. */
    adcPdcPacket.ul_addr = (uint32_t)&adcPdcBuff;
    adcPdcPacket.ul_size = *settings->blockSize;
    pdc_enable_transfer(adcPdc, PERIPH_PTCR_RXTEN);
  }
  else
  {
    return false;
  }
  
  /* Get PDC registers base address. */
//TODO: move this and init PDC after LUT table has been declared
  daccPdc = dacc_get_pdc_base(DACC);
  if(daccPdc == NULL) return FALSE;
  /* Initialize PDC packet. */
  daccPdcPacket.ul_addr = (uint32_t)settings->DAC[0].Lut;
  daccPdcPacket.ul_size = settings->DAC[0].LutLength;
  //pdc_enable_transfer(daccPdc, PERIPH_PTCR_TXTEN);
		pdc_tx_init(daccPdc, &daccPdcPacket, &daccPdcPacket);
  
  /* Return result. */
  return true;
}


/****************************************************************************************
*                             T I M E R   U T I L I T I E S
****************************************************************************************/
/************************************************************************************//**
** \brief     Initialize timer counter 0, CH 0.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool timerInit(void)
{
    /* Enable the specified peripheral clock. */
	if(pmc_enable_periph_clk(ID_TC0))
	{
		return false;
	}
	if(pmc_enable_periph_clk(ID_TC1))
	{
		return false;
	}
  
	/* Configure TC for timer, waveform generation. */
	tc_init(TC0, 0, TC_CMR_CPCTRG | TC_CMR_WAVE | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET);
	//For DAC
	tc_init(TC0, 1,
															TC_CMR_TCCLKS_TIMER_CLOCK3	//MCK/32 -> 1Mhz
															|	TC_CMR_WAVE	//waveform mode
															|	TC_CMR_WAVSEL_UP_RC	//count up, reset on RC match
															|	TC_CMR_ACPC_TOGGLE	//toggle TIOA on RC match
															);
	
	
	/* Sets timer counter period. */
	if(!timerSetTimePeriod())
	{
		return false;
	}
	
	tc_write_rc(TC0, 1, settings->DacPeriod);
  
  /* Return result. */
  return true;
}


/************************************************************************************//**
** \brief     Sets timer counter period
** \param     Time period
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool timerSetTimePeriod(void)
{
  /* Set result to true. */
  uint8_t result = true;
  uint32_t timerFreq;
  /* Initialize variables. */
  uint32_t ul_div = 0;
  uint32_t ul_tc_clks = 0;
  uint32_t ul_sysclk = sysclk_get_cpu_hz();
  
  /* check if value is within limit. */
  if(settings->acqusitionTime > 1000000)
  {
    /* Update result */
    return false;
  }
  
		timerFreq = 1000000 / settings->acqusitionTime;
		/* Get setting for timer counter. */
		if(!tc_find_mck_divisor(timerFreq, ul_sysclk, &ul_div, &ul_tc_clks, ul_sysclk))
		{
				/* Update result */
				return false;
		}
  
		/* Disable timer counter clock. */
		//TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
		/* Configure timer counter with a clock configuration */
		TC0->TC_CHANNEL[0].TC_CMR = (TC0->TC_CHANNEL[0].TC_CMR & 0xFFFFFFF8) | ul_tc_clks;
		/* Set timer counter compare value. */
		TC0->TC_CHANNEL[0].TC_RA = ((ul_sysclk / ul_div) / timerFreq) / 2;
		TC0->TC_CHANNEL[0].TC_RC = ((ul_sysclk / ul_div) / timerFreq) / 1;
		/* Reset timer counter value. */
		TC0->TC_CHANNEL[0].TC_CV = 0;
		/* Enable timer counter clock. */
		//TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
  
		/* Return result. */
		return true;
} /*** end of timer_set_compare_time ***/


//Called when user sends StartACQ
void timerStart(void)
{
  adcHandler(true);
  tc_start(TC0, 0);
}

//Called when user sends StopACQ
void dacTimerStop(void)
{
  tc_stop(TC0, 1);
}
void dacTimerStart(void)
{
  dacHandler(true);
  tc_start(TC0, 1);
}

//Called when user sends StopACQ
void timerStop(void)
{
  tc_stop(TC0, 0);
}

/****************************************************************************************
*                        C A L L B A C K   F U N C T I O N S
****************************************************************************************/
/************************************************************************************//**
** \brief     This is a callback function and it is called when tx buffer is empty.
**
****************************************************************************************/
void comTxEmptyCallback(uint8_t port)
{
  if(pdcAdcTransfetComplete)
  {
    if(settings->mode == BIN_MODE)
    {
      if(settings->averaging > 1)
      {
        for(uint16_t j = 0; j < *settings->blockSize; j += settings->averaging)
        {
          uint32_t measurment[4];
          for(uint8_t ch = 0; ((ch < 4) && settings->sequence[ch]); ch++)
          {
            for(uint16_t i = ch + j; i < (ch + j + settings->averaging); i += 4)
            {
              measurment[ch] += (adcPdcBuff[i][ch] & 0x0FFF);
            }
            measurment[ch] /= settings->averaging;
            adcPdcBuff[0][ch] = measurment[ch] | (adcPdcBuff[0][ch] & 0xF000);
          }
          settings->com->printBuf((uint8_t *)adcPdcBuff, 4*2);
        }          
      }
      else
      {
        settings->com->printBuf((uint8_t *)adcPdcBuff, *(settings->blockSize)*2);
      }
    }
    else
    {
      uint32_t measurment;
      for(uint8_t ch = 0; ((ch < 4) && settings->sequence[ch]); ch++)
      {
        measurment = (adcPdcBuff[0][ch] & 0x0FFF);
        if(settings->averaging > 1)
        {
          for(uint16_t i = 1; i < settings->averaging; i++)
          {
            measurment += (adcPdcBuff[i][ch] & 0x0FFF);
          }
          measurment /= settings->averaging;
        }
        measurment *= 20000;
        measurment /= 4095;
        measurment = 10000 - measurment;
        settings->com->len = sprintf((char*)settings->com->buf, "CH%u: %+ 6.0fmV, ",
                                     settings->sequence[ch], (measurment - 10000.0));
        settings->com->printBuf(settings->com->buf, settings->com->len);
      }
      settings->com->len = sprintf((char*)settings->com->buf, "\n\r");
      settings->com->printBuf(settings->com->buf, settings->com->len);      
    }
    pdcAdcTransfetComplete = false;
  }
}

void DACC_Handler(void)
{
	//confirm that the "end of transmit buffer interrupt" fired
	if (dacc_get_interrupt_status(DACC) & DACC_ISR_ENDTX)
	{
		//re-configure the PDC so that sine-wave generation continues
		pdc_tx_init(daccPdc, &daccPdcPacket, &daccPdcPacket);
	}
}
/************************************************************************************//**
** \brief    
** \return    Pointer to the core tx empty callback function. 
**
****************************************************************************************/
void * coreTxEmptyCallBack(void)
{
  return comTxEmptyCallback;
}


/****************************************************************************************
*                        G E N E R I C   U T I L I T I E S
****************************************************************************************/


/************************************ end of core.c ***********************************/