/*
 * parser.h
 *
 * Created: 4.3.2015 12:29:51
 *  Author: Matjaz
 */ 

#include "stdint-gcc.h"

#ifndef PARSER_H_
#define PARSER_H_

#define TRUE	1
#define FALSE	0

//Packet number definitions
#define DAQ_START				0x10		
#define DAQ_STOP				0x20
#define DAQ_SET_FREQUENCY		0x11
#define DAQ_SET_SEQUENCER		0x18
#define DAQ_SET_SAMPLE_NBR		0x12
#define DAQ_SET_CYCLE_NBR		0x22
#define DAQ_SET_ANALOG_OUT		0x32

//Finite state machine states definitions 

#define FSM_ID_BYTE				0
#define	FSM_WAIT_2_BYTES		1
#define FSM_WAIT_8_BYTES		2
#define FSM_SET_FREQ			3

#define HOLDING_BUFFER_SIZE		20

#define LIST_OF_KNOWN_COMANDS	"STRAFEDs"

#define COMAND_START_ACQ						'S'
#define COMAND_STOP_ACQ							'T'
#define COMAND_SET_SAMPLE_PERIOD				'R'
#define COMAND_SET_AVERAGE_COUNT				'A'
#define COMAND_SET_MEASURMENT_NBR_COUNT			'F'
#define COMAND_SET_SEQUENCER					'E'
#define COMAND_SET_DAC_VALUE					'D'
#define COMAND_START_FAST_ACQ					's'


#define ASCII_MODE	0
#define FAST_MODE	1



void parse_comands(void);
void init_daq_settings_struct (void);
daq_settings_t * get_current_DAQ_settings (void);

#endif /* PARSER_H_ */