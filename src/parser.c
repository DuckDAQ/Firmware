/*
 * parser.c
 *
 * Created: 4.3.2015 12:29:30
 *  Author: Matjaz
 */ 

#include <asf.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"
#include "core.h"


daq_settings_t daqSettings;


void init_daq_settings_struct (void)
{
	daqSettings.avgCounter = 1;
	daqSettings.comMode = ASCII_MODE;
	daqSettings.cycles = 1;
	daqSettings.newData = 0;
	daqSettings.sequence[0] = 1;
	daqSettings.sequence[1] = 0;
	daqSettings.timerBase = 50000;
}


void skip_blank_chars (uint8_t *string)
{
	string++;
	while((*string) == ' ' )
	{
		string++;
	}
}


void print_formating_error(void)
{
	uint32_t charsPrinted;
	uint8_t printBuffer[20];
	
	charsPrinted = sprintf(printBuffer, "Comand format error!\n\r");
	udi_cdc_write_buf(printBuffer, charsPrinted);
}

void parse_comands (void)
{
	static uint8_t fsmState = FSM_ID_BYTE;
	static uint8_t comandByte = 0;
	static uint8_t insertPointer = 0;
	uint8_t temp, n = 0;
	static uint8_t holdingBuffer[HOLDING_BUFFER_SIZE];
	static uint8_t tempBuffer[10];
	uint8_t *startOfData;
	uint8_t printBuffer [50];
	uint32_t charsPrinted, entryCounter;
	volatile int32_t a, dacCh = 0;
	
	if(udi_cdc_is_rx_ready())
	{
		temp = udi_cdc_getc();
		udi_cdc_putc(temp);
		
		if(temp == 127) // 127 = backspace in ASCII?? (sholud be DELITE)
		{
			if(insertPointer)
			{
				insertPointer--;
			}
		}
		else if(insertPointer < (HOLDING_BUFFER_SIZE - 2))
		{
			holdingBuffer[insertPointer] = temp;
			insertPointer++;
		}	
		
		if(temp == '\r')
		{
			udi_cdc_putc('\n');
			udi_cdc_putc('\r');
			
			holdingBuffer[insertPointer] = 0;
			startOfData = strpbrk(holdingBuffer, LIST_OF_KNOWN_COMANDS);
			//after this executes startOfData should point to first know character in string
			switch (*(startOfData))
			{
				case COMAND_START_ACQ:
				case COMAND_START_FAST_ACQ:
					if(*startOfData == COMAND_START_ACQ)
					{
						daqSettings.startAcq = 1;
						daqSettings.comMode = ASCII_MODE;
						charsPrinted = sprintf(printBuffer, "Acquisition started\n\r");
						udi_cdc_write_buf(printBuffer, charsPrinted);
						aquisition_start();
						break;
					}
					else if (*startOfData == COMAND_START_FAST_ACQ)
					{
						daqSettings.startAcq = 1;
						daqSettings.comMode = FAST_MODE;
						aquisition_start();
						break;
					}
					
				
				case COMAND_STOP_ACQ:
					daqSettings.stopAcq = 1;
					charsPrinted = sprintf(printBuffer, "Acquisition stoped\n\r");
					udi_cdc_write_buf(printBuffer, charsPrinted);
					aquisition_stop();
					break;
					
				case COMAND_SET_SAMPLE_PERIOD:
				case COMAND_SET_AVERAGE_COUNT:
				case COMAND_SET_MEASURMENT_NBR_COUNT:
					comandByte = *startOfData;
					//skip_blank_chars(startOfData);
					startOfData++;
					n = 0;
					while(*startOfData >= '0' && *startOfData <= '9')  //copy all numeric chars in tempBuffer
					{
						if(startOfData > (holdingBuffer + HOLDING_BUFFER_SIZE - 1)) break; // prevents from trying to read beyond the end of holding buffer
						tempBuffer[n++] = *startOfData++;
					}
					if(comandByte == COMAND_SET_SAMPLE_PERIOD)
					{
						if(*startOfData == '\r')
						{
							tempBuffer[n] = 0;
							a = atoi(tempBuffer);
							if(a < 2) a = 2;
							if(a > 100000) a = 100000;
							daqSettings.timerBase = a / 2;
							charsPrinted = sprintf(printBuffer, "Sample period set to %u uS\n\r", daqSettings.timerBase * 2);
							udi_cdc_write_buf(printBuffer, charsPrinted);
						}
						else
						{
							print_formating_error();
						}	
					}
					else if(comandByte == COMAND_SET_AVERAGE_COUNT)
					{
						if(*startOfData == '\r')
						{
							tempBuffer[n] = 0;
							daqSettings.avgCounter = atoi(tempBuffer);
							charsPrinted = sprintf(printBuffer, "DAQ will atempt to take %u samples per channel\n\r", daqSettings.avgCounter);
							udi_cdc_write_buf(printBuffer, charsPrinted);
							//todo: limit samples per channel
						}
						else
						{
							print_formating_error();
						}
					}
					else if(comandByte == COMAND_SET_MEASURMENT_NBR_COUNT)
					{
						if(*startOfData == '\r')
						{
							tempBuffer[n] = 0;
							daqSettings.cycles = atoi(tempBuffer);
							charsPrinted = sprintf(printBuffer, "DAQ will sample all enabled channels %u times\n\r", daqSettings.cycles);
							udi_cdc_write_buf(printBuffer, charsPrinted);
							//todo: limit samples per channel
						}
						else
						{
							print_formating_error();
						}
					}
					break;
				
				case COMAND_SET_SEQUENCER:
					//skip_blank_chars();
					startOfData++;
					entryCounter = 0;
					n = 0;
					while(entryCounter < 8)
					{
						n = 0;
						while(*startOfData != ',')
						{
							tempBuffer[n++] = *startOfData++;
							if(*startOfData == '\r') break;
						}
						tempBuffer[n]  = 0;
						a = atoi(tempBuffer);
						if(a)
						{
							if(a > 34 {a = 4;} // we only have 4 channels
							daqSettings.sequence[entryCounter] = a;
						}
						else
						{
							daqSettings.sequence[entryCounter] = 0;
							break;
						}
						if(*startOfData == '\r') break;
						entryCounter++;
						startOfData++;
					}
					entryCounter++;
					daqSettings.sequence[entryCounter] = 0;
					charsPrinted = sprintf(printBuffer, "Sequence set to: ");
					udi_cdc_write_buf(printBuffer, charsPrinted);
					for(n = 0; n < 8; n++)
					{
						charsPrinted = sprintf(printBuffer, "%u ", daqSettings.sequence[n]);
						udi_cdc_write_buf(printBuffer, charsPrinted);
						if(daqSettings.sequence[n] == 0) break;	
					}
					udi_cdc_putc('\n');
					udi_cdc_putc('\r');
					
					break;
					
				case COMAND_SET_DAC_VALUE:
					n = 0;
					startOfData++;
					if(*startOfData == '0') {dacCh = 0;}
					else if(*startOfData == '1') {dacCh = 1;}
					else {dacCh = 0;}
					startOfData++;
					if(*startOfData != ',') 
					{
						print_formating_error();
						break;
					}

					startOfData++;
					n = 0;
					while(*startOfData >= '0' && *startOfData <= '9' || *startOfData == '-')
					{
						if(startOfData > (holdingBuffer + HOLDING_BUFFER_SIZE - 1)) break;
						tempBuffer[n++] = *startOfData++;
					}
					tempBuffer[n] = 0;
					if(*startOfData == '\r')
					{
						a = atoi(tempBuffer);
						if(a < -10000) a = -10000;
						if(a > 10000) a = 10000;
						a = (a * 1000) / DAC_GAIN;
						a += 2048;
						dac_set(dacCh, a);
						charsPrinted = sprintf(printBuffer, "DAC channel %u set to %d mV\n\r", dacCh, ((a * DAC_GAIN) / 1000) - 10000);
						udi_cdc_write_buf(printBuffer, charsPrinted);
						
					}
					else
					{
						print_formating_error();
						break;
					}
					break;
					
				default:
					charsPrinted = sprintf(printBuffer, "Unknown comand!\n\r");
					udi_cdc_write_buf(printBuffer, charsPrinted);
					
					
			}
			
			insertPointer = 0;
		}
	
	}
}




daq_settings_t * get_current_DAQ_settings (void)
{
	return (&daqSettings);
}
