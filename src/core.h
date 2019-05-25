/*
* core.h
*
* Created: 13. 12. 2017 10:41:46
*  Author: matja
*/


#include <asf.h>

#ifndef CORE_H_
#define CORE_H_

//*******************************************************************Defines***************************************
//general
#define DAQ_MAX_AVG_NB	20		//maximum number of averages
#define DAQ_MAX_ACQ_NB	100		//maximum number of consecutive acquisitions


#define ADC_CORE_DEBUG	1 //set to 1 for aditional development debugging
#define ADC_RAW_DATA_SIZE  4 //maximum size of raw datra buffer for averaging

//ADC configs
#define ADC_CLK	16000000 // clock of ADC converter
#define ADC_IRQ_PRIORITY		4

//TIMER configs
#define TIMER_CH				0			//channel# of used timer
#define TIMER_IRQ_PRIORITY		4			//IRQ priotity for timer
#define TIMER_MAX				50000		// maximum value of timer
#define TIMER_DIV				2			//division factor to convert from microseconds to TC counts


#if ADC_CORE_DEBUG == 1				//definition of debugging pins (only if debugging enabled)
#define ADC_DEBUG_PIN	PIO_PA9
#define TIMER_DEBUG_PIN	PIO_PA10
#endif // ADC_CORE_DEBUG == 1


//*****************DACC**************************
#define DACC_CHANNEL0 0
#define DACC_CHANNEL1 1
#define DACC_ANALOG_CONTROL (DACC_ACR_IBCTLCH0(0x02) \
| DACC_ACR_IBCTLCH1(0x02) \
| DACC_ACR_IBCTLDACCORE(0x01))

//*************************************************************Macros**************************************************

#if ADC_CORE_DEBUG == 1				//debug pins control
#define ADC_DEBUG_PIN_SET	pio_set(PIOA, ADC_DEBUG_PIN)
#define ADC_DEBUG_PIN_CLR	pio_clear(PIOA, ADC_DEBUG_PIN)
#define TIMER_DEBUG_PIN_SET	pio_set(PIOA, TIMER_DEBUG_PIN)
#define TIMER_DEBUG_PIN_CLR	pio_clear(PIOA, TIMER_DEBUG_PIN)
#define ADC_DEBUG_PIN_TGL	pio_toggle_pin_group(PIOA, ADC_DEBUG_PIN)
#define TIMER_DEBUG_PIN_TGL	pio_toggle_pin_group(PIOA, TIMER_DEBUG_PIN)
#endif // ADC_CORE_DEBUG == 1

#define US_TO_TC(x) x / TIMER_DIV
//********************************************************Typedefs****************************************************

typedef enum
{
	CORE_STOPED,
	CORE_RUNNING
}core_status_t;

typedef struct
{
	uint16_t acqusitionTime;	//period of acquisition
	uint16_t acquisitionNbr;	//number of consecutive acquisitions
	uint16_t averaging;			//number of averages
	int16_t DACval[2];			//value of DAC. We have 2 DAC pins
	uint8_t DacFlag;			//When there is new DAC value to be outputed
	uint8_t binMode;			//Binary mode. ASCII_MODE=0, FAST_MODE=1
	uint8_t sequence[4];		//ADC channel read sequence. 0=no channel
	uint8_t adcGain;			//Gain of ADC
}daq_settings_t;

typedef struct
{
	uint16_t results[4];
	uint32_t chgannels;
	volatile uint32_t new_data;
}daq_measured_data_t;


//typedef enum //For ADC gain (register ADC->ADC_CGR). ADC channels are always differential!
//{
	//ADC_GAIN_0_5 = 0b00, //Gain 0.5
	//ADC_GAIN_1 = 0b01, //Gain 1
	//ADC_GAIN_2 = 0b10 //Gain 2
//}ADC_gain;
//**************************************************************Functions***********************************************
void core_init (void);
void timer_set_compare_time (uint32_t tim);
void core_configure (daq_settings_t *settings);
void DacSetVal(daq_settings_t *settings);
void core_start (void);
void setDAC(void);
core_status_t core_status_get (void);
uint32_t core_new_data_ready (void);
uint32_t core_new_data_claer (void);
uint16_t* core_get_raw_data_pntr (void);
uint32_t core_get_raw_data_size (void);
uint32_t core_get_enabled_ch (void);

/*
Clear adc_raw_accumulator[] array, where
*/
void core_clear_avg_acuum (void);
#if ADC_CORE_DEBUG == 1
void pio_init (void);
#endif

#endif /* CORE_H_ */