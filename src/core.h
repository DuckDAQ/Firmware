/************************************************************************************//**
* \file     core.h
* \brief    This module implements core DAQ functionality.
****************************************************************************************/
#ifndef CORE_H_
#define CORE_H_


/****************************************************************************************
* Include files
****************************************************************************************/
#include <asf.h>                     /* Atmel Software Framework include file          */
#include "comInterface.h"            /* USB interface driver                           */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* Logic states */
/** \brief Logic state true value */
#define FALSE               0
/** \brief Logic state false value */
#define TRUE                1

/* ADC configuration */
/** \brief clock for ADC converter */
#define ADC_CLK             16000000
/** \brief IRQ priority for timer */
#define ADC_IRQ_PRIORITY    4
/** \brief ADC gain 0.5 register value */
#define ADC_GAIN_0_5        0
/** \brief ADC gain 1.0 register value */
#define ADC_GAIN_1          1
/** \brief ADC gain 2.0 register value */
#define ADC_GAIN_2          2
/** \brief DAQ ADC chanell 1 to uC ADC chanell map */
#define DAQ_CH_1_ADC_CH     4
/** \brief DAQ ADC chanell 2 to uC ADC chanell map */
#define DAQ_CH_2_ADC_CH     6
/** \brief DAQ ADC chanell 3 to ADC uC chanell map */
#define DAQ_CH_3_ADC_CH     0
/** \brief DAQ ADC chanell 4 to ADC uC chanell map */
#define DAQ_CH_4_ADC_CH     2
/** \brief Size of ADC buffer for measurments */
#define ADC_BUFFER_SIZE     256

/* DAC configuration */
/** \brief DAQ DAC chanell 1 to uC DACC chanell map */
#define DACC_CHANNEL0       0
/** \brief DAQ DAC chanell 2 to uC DACC chanell map */
#define DACC_CHANNEL1       1
/** \brief Size of DAC buffer for look up table */
#define DACC_BUFFER_SIZE    1024
/** \brief IRQ priority for timer */
#define DACC_IRQ_PRIORITY   4

/* TIMER configuration */
/** \brief channel# of used timer */
#define TIMER_CH            0
/** \brief IRQ priority for timer */
#define TIMER_IRQ_PRIORITY  4
/** \brief maximum value of timer */
#define TIMER_MAX           50000
/** \brief division factor to convert from microseconds */
#define TIMER_DIV           2
/** \brief to TC counts */
#define US_TO_TC(x)         (x / TIMER_DIV)
/** \brief TC divisor used to find the lowest acceptable
 *         timer frequency
 */
#define TC_DIV_FACTOR       0xFFFF         

/* Communication mode defines */
/** \brief Parameter ASCII mode value */
#define ASCII_MODE          0
/** \brief Parameter BIN mode value */
#define BIN_MODE            1


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief DAQ structure with all the settings */
typedef struct                       /* DAQ settings type                              */
{
  uint32_t acqusitionTime;           /* Time period of acquisitions                    */
  uint16_t acquisitionNbr;           /* Number of consecutive acquisitions             */
  uint16_t averaging;                /* Number of averages                             */
  uint8_t ADCgain[4];                /* ADC gain for each channel                      */
  uint8_t syncBytes[2];              /* Sync bytes marking start of block              */
  uint8_t ADClowRes;                 /* ADC enable low resoultion mode, 8-bit.         */
  uint8_t sequence[4];               /* ADC channel read sequence. 0 = no channel      */
  uint8_t mode;                      /* Output mode. ASCII_MODE=0, BIN_MODE=1          */
  uint8_t DACgain;                   /* Gain of ADC                                    */
  uint32_t * blockSize;              /* Size of block                                  */
  uint16_t DACval[2];                /* DAC channel output value                       */
  uint16_t DacPeriod;                /* This will define how fast new value will be
                                      * inserted do DAC FIFO. Period of TC Channel1 is
                                      * microseconds (Channel0 is for ADC)
                                      */
  uint16_t Lut[DACC_BUFFER_SIZE*2];  /* Buffer for LUT values. Will be used by PDC.
                                      * Lut values for both channels. Even number
                                      * indexes = one channel,
                                      * odd num indexes = other channel
                                      */
  uint16_t LutLength;                /* How many values does LUT have                  */
  uint16_t NumOfRepeats;             /* Number of LUT repeats, 0 = continuous, 65k max */
  uint16_t CurrentRepeats;           /* Current number of LUT repeats                  */
  COM_t *com;                        /* Pointer to the linked communication interface  */
}daq_settings_t;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* Core functions */
bool coreConfigure (daq_settings_t * master_settings);
bool coreStart(void);

/* Communication callback function pointer. */
void * coreGetTxEmptyCallBackPtr(void);

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
void setDacPeriod(void);
void SetDacPdcLength(void);
void setDacTransferMode(uint8_t val);
uint32_t GetLutCntr(void);


#endif /* CORE_H_ */
/************************************ end of core.h ***********************************/