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

#define TRUE	1
#define FALSE	0

//Packet number definitions
#define DAQ_START				    0x10		
#define DAQ_STOP				    0x20
#define DAQ_SET_FREQUENCY		0x11
#define DAQ_SET_SEQUENCER		0x18
#define DAQ_SET_SAMPLE_NBR  0x12
#define DAQ_SET_CYCLE_NBR		0x22
#define DAQ_SET_ANALOG_OUT	0x32

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
#define DAC_VALUE_PAR0_LOWRANGE  1
#define DAC_VALUE_PAR0_HIGHRANGE 4
#define DAC_VALUE_PAR1_LOWRANGE -10000
#define DAC_VALUE_PAR1_HIGHRANGE 10000

#define ASCII_MODE  0
#define FAST_MODE	  1

typedef struct
{
  uint8_t cmd;
  bool (*funcPtr)(int32_t*, daq_settings_t*);
  int32_t par[4];
}CMD_t;

bool parseCommand (uint8_t CMD, CMD_t *parsedCMD);
bool getPar(uint8_t parCount, uint16_t timeout, int32_t *parPtr);

bool StartACQ (int32_t *parPtr, daq_settings_t *settings);
bool StartFastACQ (int32_t *parPtr, daq_settings_t *settings);
bool StopACQ (int32_t *parPtr, daq_settings_t *settings);
bool SetSamplePeriod (int32_t *parPtr, daq_settings_t *settings);
bool SetAverageCount (int32_t *parPtr, daq_settings_t *settings);
bool SetMeasurmentCount (int32_t *parPtr, daq_settings_t *settings);
bool SetSequencer (int32_t *parPtr, daq_settings_t *settings);
bool SetDACvalue (int32_t *parPtr, daq_settings_t *settings);

#endif /* PARSER_H_ */