/************************************************************************************//**
* \file     core.h
* \brief    This module implements core DAQ functionality.
****************************************************************************************/
#ifndef CORE_H_
#define CORE_H_


/****************************************************************************************
* Include files
****************************************************************************************/
#include <asf.h>
#include "comInterface.h"

/****************************************************************************************
* Macro definitions
****************************************************************************************/

#define ADC_CORE_DEBUG	  1          /* set to 1 for additional development debugging  */
#define ADC_RAW_DATA_SIZE 4          /* maximum size of raw data buffer for averaging  */

/* ADC configuration */
#define ADC_CLK	16000000             /* channel# of used timer                         */
#define ADC_IRQ_PRIORITY		4

/* TIMER configuration */
#define TIMER_CH				    0        /* channel# of used timer                         */
#define TIMER_IRQ_PRIORITY  4	       /* IRQ priority for timer                         */
#define TIMER_MAX				    50000    /* maximum value of timer                         */
#define TIMER_DIV				    2	       /* division factor to convert from microseconds   */
                                     /* to TC counts                                   */

#define US_TO_TC(x) x / TIMER_DIV
                                            
/* Definition of debugging pins (only if debugging enabled) */
#if ADC_CORE_DEBUG == 1
	#define ADC_DEBUG_PIN	PIO_PA9
	#define TIMER_DEBUG_PIN	PIO_PA10
#endif

/* debug pins control */
#if ADC_CORE_DEBUG == 1
	#define ADC_DEBUG_PIN_SET	pio_set(PIOA, ADC_DEBUG_PIN)
	#define ADC_DEBUG_PIN_CLR	pio_clear(PIOA, ADC_DEBUG_PIN)
	#define TIMER_DEBUG_PIN_SET	pio_set(PIOA, TIMER_DEBUG_PIN)
	#define TIMER_DEBUG_PIN_CLR	pio_clear(PIOA, TIMER_DEBUG_PIN)
	#define ADC_DEBUG_PIN_TGL	pio_toggle_pin_group(PIOA, ADC_DEBUG_PIN)
	#define TIMER_DEBUG_PIN_TGL	pio_toggle_pin_group(PIOA, TIMER_DEBUG_PIN)
#endif

/* Logic states */
#define FALSE 0
#define TRUE  1


/****************************************************************************************
* Type definitions
****************************************************************************************/
/* Bit mask for channel enabling. */
typedef enum
{
	DAQ_CHANNEL_1 = 0x01,
	DAQ_CHANNEL_2 = 0x02,
	DAQ_CHANNEL_3 = 0x04,
	DAQ_CHANNEL_4 = 0x08
}channel_bitmask_t;

/* Core status */
typedef enum
{
	CORE_STOPED,
	CORE_RUNNING
}core_status_t;

/* For ADC gain (register ADC->ADC_CGR). ADC channels are always differential! */
typedef enum
{
  ADC_GAIN_0_5 = 0b00,              /* Gain 0.5                                       */
  ADC_GAIN_1 = 0b01,                /* Gain 1.0                                       */
  ADC_GAIN_2 = 0b10                 /* Gain 2.0                                       */
}ADC_gain;

/* DAQ settings */
typedef struct
{
	uint16_t acqusitionTime;          /* period of acquisition acquisitions             */
	uint16_t acquisitionNbr;          /* number of consecutive acquisitions             */
	uint16_t averaging;               /* number of averages                             */
  int16_t DACval[4];                /* channel# of used timer                         */
	channel_bitmask_t channels;       /* enabled channels bit mask                      */
  uint8_t binMode;
  uint8_t sequence[4];
  ADC_gain adcGain;
  COM_t *com;
}daq_settings_t;

/* DAQ measured data */
typedef struct
{
	uint16_t results[4];
	uint32_t chgannels;
	volatile uint32_t new_data;
}daq_measured_data_t;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
void core_init (void);
void timer_set_compare_time (uint32_t tim);
bool core_configure (daq_settings_t *settings);
void core_start (void);
core_status_t core_status_get (void);
uint32_t core_new_data_ready (void);
uint32_t core_new_data_claer (void);
uint32_t *core_get_raw_data_pntr (void);
uint32_t core_get_raw_data_size (void);
uint32_t core_get_enabled_ch (void);
void core_clear_avg_acuum (void);

//Clear adc_raw_accumulator[] array, where

/* debug pins control */
#if ADC_CORE_DEBUG == 1
  void pio_init (void);
#endif
  
#endif /* CORE_H_ */
/************************************ end of core.h ***********************************/