/*
 * parser.h
 *
 * Created: 4.3.2015 12:29:51
 *  Author: Matjaz
 */ 

#include "stdint-gcc.h"

#ifndef PARSER_H_
#define PARSER_H_

#include "core.h"
#include "comInterface.h"

#define TRUE	1
#define FALSE	0

// Command list
#define CMD_START_ACQ			        'S'
#define CMD_STOP_ACQ			        'T'
#define CMD_SET_SAMPLE_PERIOD	    'R'
#define CMD_SET_AVERAGE_COUNT	    'A'
#define CMD_SET_MEASURMENT_COUNT  'F'
#define CMD_SET_SEQUENCER		      'E'
#define CMD_SET_DAC_VALUE		      'D'
#define CMD_START_FAST_ACQ		    's'

#define MAX_PARAMETER_COUNT 4
#define MAX_PARAMETER_LENGHT 7
#define PARAMETER_TIMEOUT 100

#define SAMPLE_PERIOD_LOWRANGE  10
#define SAMPLE_PERIOD_HIGHRANGE 50000
#define AVERAGE_COUNT_LOWRANGE  1
#define AVERAGE_COUNT_HIGHRANGE 1000
#define MEASURMENT_COUNT_LOWRANGE  1
#define MEASURMENT_COUNT_HIGHRANGE 1000
#define SEQUENCER_LOWRANGE  0
#define SEQUENCER_HIGHRANGE 4
#define DAC_VALUE_PAR0_LOWRANGE  0 //channel 0
#define DAC_VALUE_PAR0_HIGHRANGE 1 //channel 1
#define DAC_VALUE_PAR1_LOWRANGE 0 // = -9.74V
#define DAC_VALUE_PAR1_HIGHRANGE 4095 // = 9.68V

#define ASCII_MODE  0
#define FAST_MODE	  1

typedef struct
{
  uint8_t cmd;
  bool (*funcPtr)(int32_t*, daq_settings_t*, COM_t *comInterface);
  int32_t par[4];
}CMD_t;

bool parseCommand (uint8_t CMD, CMD_t *parsedCMD, COM_t *comInterface);
bool getPar(uint8_t parCount, uint16_t timeout, int32_t *parPtr, COM_t *comInterface);

bool StartACQ (int32_t *parPtr, daq_settings_t *settings,  COM_t *comInterface);
bool StartFastACQ (int32_t *parPtr, daq_settings_t *settings,  COM_t *comInterface);
bool StopACQ (int32_t *parPtr, daq_settings_t *settings,  COM_t *comInterface);
bool SetSamplePeriod (int32_t *parPtr, daq_settings_t *settings,  COM_t *comInterface);
bool SetAverageCount (int32_t *parPtr, daq_settings_t *settings,  COM_t *comInterface);
bool SetMeasurmentCount (int32_t *parPtr, daq_settings_t *settings,  COM_t *comInterface);
bool SetSequencer (int32_t *parPtr, daq_settings_t *settings,  COM_t *comInterface);
bool SetDACvalue (int32_t *parPtr, daq_settings_t *settings,  COM_t *comInterface);

#endif /* PARSER_H_ */