/************************************************************************************//**
* \file     parser.h
* \brief    This module implements parser for DAQ commands.
****************************************************************************************/
#ifndef PARSER_H_
#define PARSER_H_


/****************************************************************************************
* Include files
****************************************************************************************/
#include "core.h"
#include "comInterface.h"


/****************************************************************************************
* Macro definitions
****************************************************************************************/
/* Logic states */
#define TRUE  1
#define FALSE 0
/* supported commands */
#define CMD_START_ACQ				'S'
#define CMD_STOP_ACQ				'T'
#define CMD_SET_AVERAGE_COUNT		'A'
#define CMD_SET_SAMPLE_PERIOD		'R'
#define CMD_SET_MEASURMENT_COUNT	'F'
#define CMD_SET_SEQUENCER			'E'
#define CMD_SET_MODE				'M'
#define CMD_SET_ADC_GAIN			'G'
#define CMD_SET_ADC_LOW_RESOLUTION	'L'
#define CMD_SET_BLOCK_SIZE			'B'
//DAC commands
#define CMD_SET_REPEAT_NUM			'C'
#define CMD_SET_DAC_FREQ		'H'
#define CMD_SET_DAC_LUT				'J'
#define CMD_DAC_LUT_LENGTH 'N'
#define CMD_DAC_START						'O'
#define CMD_DAC_STOP							'K'
#define CMD_DAC_TRANSFER_MODE 'P'
#define CMD_DAC_LUT_COUNTER 'U'
/* Parameter limits */
#define MAX_PARAMETER_COUNT   4
#define MAX_PARAMETER_LENGHT  7
#define PARAMETER_TIMEOUT     100
/* Parameter ranges */
#define MODE_LOWRANGE               0
#define MODE_HIGHRANGE              1
#define SAMPLE_PERIOD_LOWRANGE      1
#define SAMPLE_PERIOD_HIGHRANGE     1000000
#define AVERAGE_COUNT_LOWRANGE      1
#define AVERAGE_COUNT_HIGHRANGE     1000
#define MEASURMENT_COUNT_LOWRANGE   0
#define MEASURMENT_COUNT_HIGHRANGE  1000
#define SEQUENCER_LOWRANGE          0
#define SEQUENCER_HIGHRANGE         4
#define DAC_VALUE_PAR1_LOWRANGE    -10000
#define DAC_VALUE_PAR1_HIGHRANGE    10000
#define ADC_RES_12_BITS             0
#define ADC_RES_10_BITS             1
#define BLOCK_SIZE_MIN              1
#define BLOCK_SIZE_MAX              1024

#define DAC_CH_LOWRANGE				1
#define DAC_CH_HIGHRANGE			2
#define DAC_VALUE_MIN				0
#define DAC_VALUE_MAX			 8191 //12th bit is channel selection bit. Can be 1.
#define DAC_LUT_LOCATION_LOWRANGE	0
#define DAC_LUT_LOCATION_HIGHRANGE 1023
#define DAC_REPEAT_NUM_MIN			0
#define DAC_REPEAT_NUM_MAX			65500
#define DAC_PERIOD_MIN			1
#define DAC_PERIOD_MAX			65500

/* communication mode defines */
#define ASCII_MODE  0
#define BIN_MODE    1


/****************************************************************************************
* Type definitions
****************************************************************************************/
typedef struct
{
  uint8_t cmd;
  bool (*funcPtr)(int32_t*, daq_settings_t*, COM_t *comInterface);
  int32_t par[4];
}CMD_t;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* Parser functions */
bool parseCommand (uint8_t CMD, CMD_t *parsedCMD, COM_t *comInterface);


#endif /* PARSER_H_ */
/*********************************** end of parser.h ***********************************/