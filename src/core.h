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
#define ADC_CLK	          16000000   /* clock for ADC converter                        */
#define ADC_IRQ_PRIORITY	4

/* DAC configuration */
#define DACC_CHANNEL0 0
#define DACC_CHANNEL1 1
#define DACC_ANALOG_CONTROL (DACC_ACR_IBCTLCH0(0x02) \
| DACC_ACR_IBCTLCH1(0x02) \
| DACC_ACR_IBCTLDACCORE(0x01))

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
  ADC_GAIN_1  = 0b01,               /* Gain 1.0                                       */
  ADC_GAIN_2  = 0b10                /* Gain 2.0                                       */
}ADC_gain;

/* DAQ settings */
typedef struct
{
	uint16_t acqusitionTime;          /* time period of acquisitions                    */
	uint16_t acquisitionNbr;          /* number of consecutive acquisitions             */
	uint16_t averaging;               /* number of averages                             */
  int16_t DACval[2];                /* channel# of used timer                         */
	uint8_t DACflag;			            /* When there is new DAC value to be outputed     */
  uint8_t DACgain;			            /* Gain of ADC                                    */
  uint8_t sequence[4];              /* ADC channel read sequence. 0=no channel        */
  uint8_t binMode;                  /* Binary mode. ASCII_MODE=0, FAST_MODE=1         */
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
void core_clear_avg_acuum (void);
void DacSetVal(daq_settings_t *settings);
void setDAC(void);

//Clear adc_raw_accumulator[] array, where

/* debug pins control */
#if ADC_CORE_DEBUG == 1
  void pio_init (void);
#endif
  
#endif /* CORE_H_ */
/************************************ end of core.h ***********************************/