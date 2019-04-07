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
#define PARAMETER_TIMEOUT 100

#define ASCII_MODE  0
#define FAST_MODE	  1

typedef struct
{
  uint8_t cmd;
  bool (*funcPtr)(uint16_t*, daq_settings_t*);
  uint16_t par[4];
}CMD_t;

bool parseCommand (uint8_t CMD, CMD_t *parsedCMD);
bool getPar(uint8_t parCount, uint16_t timeout, CMD_t *CMDpar);

bool StartACQ (uint16_t *parPtr, daq_settings_t *settings);
bool StartFastACQ (uint16_t *parPtr, daq_settings_t *settings);
bool StopACQ (uint16_t *parPtr, daq_settings_t *settings);
bool SetSamplePeriod (uint16_t *parPtr, daq_settings_t *settings);
bool SetAverageCount (uint16_t *parPtr, daq_settings_t *settings);
bool SetMeasurmentCount (uint16_t *parPtr, daq_settings_t *settings);
bool SetSequencer (uint16_t *parPtr, daq_settings_t *settings);
bool SetDACvalue (uint16_t *parPtr, daq_settings_t *settings);

#endif /* PARSER_H_ */