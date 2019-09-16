/************************************************************************************//**
* \file     parser.h
* \brief    This module implements parser for DAQ commands.
****************************************************************************************/
#ifndef PARSER_H_
#define PARSER_H_


/****************************************************************************************
* Include files
****************************************************************************************/
#include "core.h"                            /* Core functionality for acquisition     */
#include "comInterface.h"                    /* USB interface driver                   */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* Logic states */
/** \brief Logic state true value */
#define TRUE  1
/** \brief Logic state false value */
#define FALSE 0

/* Supported commands */
/** \brief Command will start ADC sampling. */
#define CMD_START_ACQ               'S'
/** \brief Command will stop ADC sampling. */
#define CMD_STOP_ACQ                'T'
/** \brief Command will set number of averageing */
#define CMD_SET_AVERAGE_COUNT       'A'
/** \brief Command will set time between samples */
#define CMD_SET_SAMPLE_PERIOD       'R'
/** \brief Command will set number of samples */
#define CMD_SET_MEASURMENT_COUNT    'F'
/** \brief Command will set sequence of channels */
#define CMD_SET_SEQUENCER           'E'
/** \brief Command will set ADC mode ASCII or BIN */
#define CMD_SET_MODE                'M'
/** \brief Command will set ADC gain for channel */
#define CMD_SET_ADC_GAIN            'G'
/** \brief Command will set ADC in low resolution */
#define CMD_SET_ADC_LOW_RESOLUTION  'L'
/** \brief Command will set size of the block */
#define CMD_SET_BLOCK_SIZE          'B'
/* DAC commands */
/** \brief Command will set number of LUT repeats */
#define CMD_SET_REPEAT_NUM          'C'
/** \brief Command will set DAC LUT sample timing */
#define CMD_SET_DAC_FREQ            'H'
/** \brief Command will set DAC LUT sample value */
#define CMD_SET_DAC_LUT             'J'
/** \brief Command will set DAC LUT length */
#define CMD_DAC_LUT_LENGTH          'N'
/** \brief Command will start DAC sampling. */
#define CMD_DAC_START               'O'
/** \brief Command will stop DAC sampling. */
#define CMD_DAC_STOP                'K'
/** \brief Command will set DAC tranfer mode */
#define CMD_DAC_TRANSFER_MODE       'P'
/** \brief Command will send DAC LUT counter value */
#define CMD_DAC_LUT_COUNTER         'U'

/* Parameter limits */
/** \brief Maximum number of parameters supported */
#define MAX_PARAMETER_COUNT         4
/** \brief Maximum parameter length "9999999" */
#define MAX_PARAMETER_LENGHT        7
/** \brief Maximum time to wait for all parameters*/
#define PARAMETER_TIMEOUT           100

/* Parameter ranges */
/** \brief Parameter mode minimum value */
#define MODE_LOWRANGE               0
/** \brief Parameter mode maximum value */
#define MODE_HIGHRANGE              1
/** \brief Parameter sample period minimum value */
#define SAMPLE_PERIOD_LOWRANGE      1
/** \brief Parameter sample period maximum value */
#define SAMPLE_PERIOD_HIGHRANGE     1000000
/** \brief Parameter minimum number of averages */
#define AVERAGE_COUNT_LOWRANGE      1
/** \brief Parameter maximum number of averages */
#define AVERAGE_COUNT_HIGHRANGE     1000
/** \brief Parameter minimum number of samples */
#define MEASURMENT_COUNT_LOWRANGE   0
/** \brief Parameter maximum number of samples */
#define MEASURMENT_COUNT_HIGHRANGE  1000
/** \brief Parameter sequencer minimum value */
#define SEQUENCER_LOWRANGE          0
/** \brief Parameter sequencer maximum value */
#define SEQUENCER_HIGHRANGE         4
/** \brief Parameter minimum value */
#define DAC_VALUE_PAR1_LOWRANGE    -10000
/** \brief Parameter maximum value */
#define DAC_VALUE_PAR1_HIGHRANGE    10000
/** \brief Parameter ADC resolution 12-bit value */
#define ADC_RES_12_BITS             0
/** \brief Parameter ADC resolution 10-bit value */
#define ADC_RES_10_BITS             1
/** \brief Parameter minimum block size */
#define BLOCK_SIZE_MIN              1
/** \brief Parameter maximum block size */
#define BLOCK_SIZE_MAX              1024
/** \brief Parameter minimum DAC chanell */
#define DAC_CH_LOWRANGE             1
/** \brief Parameter maximum DAC chanell */
#define DAC_CH_HIGHRANGE            2
/** \brief Parameter DAC value minimum value */
#define DAC_VALUE_MIN               0
/** \brief 12th bit is channel selection bit. Can be 1. */
#define DAC_VALUE_MAX               8191
/** \brief Paramater LUT locaction minimum value */
#define DAC_LUT_LOCATION_LOWRANGE   0
/** \brief Paramater LUT locaction maximum value */
#define DAC_LUT_LOCATION_HIGHRANGE  1023
/** \brief Parameter min number of DAC LUT repeats */
#define DAC_REPEAT_NUM_MIN          0
/** \brief Parameter max number of DAC LUT repeats */
#define DAC_REPEAT_NUM_MAX          0xFFFF
/** \brief Parameter DAC minimum sample period */
#define DAC_PERIOD_MIN              1
/** \brief Parameter DAC maximum sample period */
#define DAC_PERIOD_MAX              0xFFFF

/* Communication mode defines */
/** \brief Parameter ASCII mode value */
#define ASCII_MODE                  0
/** \brief Parameter BIN mode value */
#define BIN_MODE                    1


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Command structure */
typedef struct
{
  uint8_t cmd;                               /* Command indentifier                    */
                                             /* Function pointer to the command handler*/
  bool (*funcPtr)(int32_t*, daq_settings_t*, COM_t *comInterface);
  int32_t par[MAX_PARAMETER_COUNT];          /* Parameters values                      */
}CMD_t;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* Parser function */
bool parseCommand (uint8_t CMD, CMD_t *parsedCMD, COM_t *comInterface);


#endif /* PARSER_H_ */
/*********************************** end of parser.h ***********************************/