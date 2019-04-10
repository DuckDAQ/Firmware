/*
 * core.c
 *
 * Created: 13. 12. 2017 10:41:36
 *  Author: matja
 */
#include <asf.h>
#include "core.h"

//PDC(peripheral DMA controller) data packet for transfer. Nastavimo mu start in size
pdc_packet_t adc_pdc1, adc_pdc2;
//Pdc hardware registers
Pdc *adc_pdc_pntr;

uint16_t adc_raw_data1 [ADC_RAW_DATA_SIZE];
uint16_t adc_raw_data2 [ADC_RAW_DATA_SIZE];
uint32_t adc_raw_accumulator [ADC_RAW_DATA_SIZE];
volatile uint32_t rep_cntr, new_data = 0, acqusition_in_progress = 0, data_bank = 0, avg_cntr, avg_cnt_reload;
uint32_t raw_data_size;
uint32_t nb_enables_ch;


uint32_t core_get_enabled_ch (void)
{
	return nb_enables_ch;
}


#if ADC_CORE_DEBUG == 1
	void pio_init (void)
	{
		pmc_enable_periph_clk(ID_PIOA);
		pio_set_output(PIOA, ADC_DEBUG_PIN, LOW, DISABLE, DISABLE); //indicator pin for ADC
		pio_set_output(PIOA, TIMER_DEBUG_PIN, LOW, DISABLE, DISABLE);//indicator pin for Timer0
	}
#endif //ADC_CORE_DEBUG == 1




void core_init (void)
{
	//init adc
	pmc_enable_periph_clk(ID_ADC);
	adc_init(ADC, sysclk_get_cpu_hz(), ADC_CLK, 0);
	adc_configure_timing(ADC, 15, ADC_SETTLING_TIME_0, 0);
	adc_configure_trigger(ADC, ADC_TRIG_SW, 0); //! WARNING! Bug in ASF! ADC_MR_FREERUN_ON does't actualy enables freerun mode!
	//adc_check(ADC, sysclk_get_cpu_hz());
	ADC->ADC_COR |= (ADC_COR_DIFF0 | ADC_COR_DIFF1 | ADC_COR_DIFF2 | ADC_COR_DIFF3
					 | ADC_COR_DIFF4 | ADC_COR_DIFF5 | ADC_COR_DIFF6 | ADC_COR_DIFF7); // set channels to differential
	ADC->ADC_CGR = 0x00005555; // set gain to 1
	adc_set_bias_current(ADC, 1);
	#if ADC_CORE_DEBUG == 1
		pio_init();
	#endif //ADC_CORE_DEBUG == 1

	adc_pdc_pntr = adc_get_pdc_base(ADC); // init DMA

	//Both pdc1 & pdc2:
	//	!Warning		assignment makes integer from pointer without a cast [-Wint-conversion]
	adc_pdc1.ul_addr = adc_raw_data1;
	adc_pdc2.ul_addr = adc_raw_data2;

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
}

void timer_set_compare_time (uint32_t tim)
{
	if(tim > 50000) tim = 50000;
	tc_write_rc(TC0, TIMER_CH, tim);
}


void core_configure (daq_settings_t *settings)
{
	uint32_t n;
	//volatile uint32_t dummy;

	//clear averaging accumulator
	core_clear_avg_acuum ();

	//disable all adc channels
	adc_disable_all_channel(ADC);
	//enable adc channels that are enabled in settings struct
	for(n = 0; n < 4; n++)
	{
		if(settings->channels & (0x01 << n))
		{
			nb_enables_ch ++;
			adc_enable_channel(ADC, (n * 2));
		}
	}

	//configure DMA (treba nastavit start & size)
	//raw_data_size = settings->averaging * nb_enables_ch;
	adc_pdc1.ul_size = nb_enables_ch;
	adc_pdc2.ul_size = nb_enables_ch;

	pdc_rx_init(adc_pdc_pntr, &adc_pdc1, &adc_pdc2);
	data_bank = 0;
	adc_enable_interrupt(ADC, ADC_IER_ENDRX);

	//set repetition counter
	rep_cntr = settings->acquisitionNbr;

	//set average counter
	avg_cntr = settings->averaging;
	avg_cnt_reload = settings->averaging;

	//set timer
	timer_set_compare_time(US_TO_TC(settings->acqusitionTime));

	#if ADC_CORE_DEBUG == 1
			TIMER_DEBUG_PIN_CLR;
	#endif //ADC_CORE_DEBUG == 1

}

void core_start (void)
{
	pdc_enable_transfer(adc_pdc_pntr, PERIPH_PTCR_RXTEN);
	acqusition_in_progress = 1;
	tc_start(TC0, TIMER_CH);
	//ADC->ADC_MR |= ADC_MR_FREERUN; //due to a bug in ASF we enable freerun mode manualy
	adc_start(ADC);
}

core_status_t core_status_get (void)
{
	if(acqusition_in_progress)
	{
		return CORE_RUNNING;
	}
	else
	{
		return CORE_STOPED;
	}
}


uint32_t core_new_data_ready (void)
{
	return new_data;
}

//?typo, should be core_new_data_clear
uint32_t core_new_data_claer (void)
{
	new_data = 0;
	return 0;
}

//!Warning return from incompatible pointer type [-Wincompatible-pointer-types]
uint16_t* core_get_raw_data_pntr (void)
{
	return adc_raw_accumulator;
}

uint32_t core_get_raw_data_size (void)
{
	return raw_data_size;
}

void core_clear_avg_acuum (void)
{
	uint32_t n;
	//clear averageing accumulator
	for(n = 0; n < ADC_RAW_DATA_SIZE; n++)
	{
		adc_raw_accumulator[n] = 0;
	}
}

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
			//do this to clear dma flag
			pdc_rx_init(adc_pdc_pntr, &adc_pdc1, NULL);
			//report new data
			new_data = 1;
			avg_cntr = avg_cnt_reload;
		}


	}
}

/*
?Timer/Counter 0 handler, se uporablja za ADC??
*/
void TC0_Handler (void)
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
}