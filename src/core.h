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
/* ADC configuration */
#define ADC_CLK           16000000   /* clock for ADC converter                        */
#define ADC_IRQ_PRIORITY  4
#define ADC_GAIN_0_5 0
#define ADC_GAIN_1   1
#define ADC_GAIN_2   2
#define DAQ_CH_1_ADC_CH 4
#define DAQ_CH_2_ADC_CH 6
#define DAQ_CH_3_ADC_CH 0
#define DAQ_CH_4_ADC_CH 2
#define ADC_BUFFER_SIZE 256

/* DAC configuration */
#define DACC_CHANNEL0 0
#define DACC_CHANNEL1 1
#define DACC_ANALOG_CONTROL ( DACC_ACR_IBCTLCH0(0x02) | \
                              DACC_ACR_IBCTLCH1(0x02) | \
                              DACC_ACR_IBCTLDACCORE(0x01) )
#define DACC_BUFFER_SIZE 1024
#define DACC_IRQ_PRIORITY 4

/* TIMER configuration */
#define TIMER_CH            0        /* channel# of used timer                         */
#define TIMER_IRQ_PRIORITY  4        /* IRQ priority for timer                         */
#define TIMER_MAX           50000    /* maximum value of timer                         */
#define TIMER_DIV           2        /* division factor to convert from microseconds   */
#define US_TO_TC(x) x / TIMER_DIV    /* to TC counts                                   */

/* Logic states */
#define FALSE 0
#define TRUE  1

/* communication mode defines */
#define ASCII_MODE  0
#define BIN_MODE    1

/* TC divisor used to find the lowest acceptable timer frequency */
#define TC_DIV_FACTOR 65536

//#define FIRST_SYNC_BYTE 0x9F //159
//#define SECOND_SYNC_BYTE 0xA3 //163


/****************************************************************************************
* Type definitions
****************************************************************************************/
/* DAQ settings */
typedef struct
{
  uint32_t acqusitionTime;          /* time period of acquisitions                    */
  uint16_t acquisitionNbr;          /* number of consecutive acquisitions             */
  uint16_t averaging;               /* number of averages                             */
  uint8_t ADCgain[4];
		uint8_t syncBytes[2];
  uint8_t ADClowRes;
  uint8_t DACgain;                  /* Gain of ADC                                    */
  uint8_t sequence[4];              /* ADC channel read sequence. 0=no channel        */
  uint8_t mode;                     /* Output mode. ASCII_MODE=0, BIN_MODE=1          */
  uint32_t * blockSize;
  COM_t *com;
  
  uint16_t DACval[2];               /* channel# of used timer                         */
  uint8_t DacSequence [2]; //DAC channel write sequence. 0 = no channel
		uint32_t DacFreq;
		struct DAC_struct
		{
			uint16_t Lut[DACC_BUFFER_SIZE]; //Buffer for LUT values. Will be used by PDC
			uint16_t LutLength; //How many values does LUT have
			uint16_t NumOfRepeats; //Number of LUT repeats. 0 = continuous mode. 65k max
			uint16_t CurrentRepeats; //Current number of LUT repeats
		}DAC[2];
}daq_settings_t;

//DAC settings


/***************************************************************************************
* Function prototypes
****************************************************************************************/
bool coreConfigure (daq_settings_t * master_settings);
bool coreStart(void);
/* Communication callback function pointer. */
void * coreTxEmptyCallBack(void);
/* Timer core handlers. */
bool timerSetTimePeriod(void);
void timerStart(void);
void timerStop(void);
void dacTimerStart(void);
void dacTimerStop(void);
/* ADC core handlers. */
void adcHandler(bool state);
void adcSetRes(void);
void adcSetGain(void);
void adcSetChannels(void);
bool DacSetTimer(void);
/* DAC core handlers. */
void coreSetDacVal(void);

  
#endif /* CORE_H_ */
/************************************ end of core.h ***********************************/