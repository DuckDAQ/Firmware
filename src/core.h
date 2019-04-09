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
typedef enum		// bitmask for channel enabling
{
	DAQ_CHANNEL_1 = 0x01,
	DAQ_CHANNEL_2 = 0x02,
	DAQ_CHANNEL_3 = 0x04,
	DAQ_CHANNEL_4 = 0x08
}channel_bitmask_t;

typedef enum
{
	CORE_STOPED,
	CORE_RUNNING
}core_status_t;

typedef struct
{
	uint16_t acqusitionTime;	//period of acqusition
	uint16_t acquisitionNbr;	//number of consecutive acquisitions
	uint16_t averaging;			//number of averages
  int16_t DACval[4];
	channel_bitmask_t channels;	//enabled channels bitmask
  uint8_t binMode;
  uint8_t sequence[4];
}daq_settings_t;

typedef struct
{
	uint16_t results[4];
	uint32_t chgannels;
	volatile uint32_t new_data;
}daq_measured_data_t;

//**************************************************************Functions***********************************************
void core_init (void);
void timer_set_compare_time (uint32_t tim);
void core_configure (daq_settings_t *settings);
void core_start (void);
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
/*
Preveri nastavitve DAQ, v primeru da so parametri out-of-range, jih nastavi na primerno vrednost.
*/
void validate_settings (daq_settings_t *settings);
#endif /* CORE_H_ */