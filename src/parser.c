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


bool parseCommand (uint8_t CMD, CMD_t *parsedCMD)
{
	bool result = FALSE;	//set result to false
	
	switch (CMD)
	{
  	case CMD_START_ACQ:
      parsedCMD->cmd = CMD_START_ACQ;
      parsedCMD->funcPtr = StartACQ;
      result = getPar(0, PARAMETER_TIMEOUT, parsedCMD);
      break;
  	case CMD_START_FAST_ACQ:
      parsedCMD->cmd = CMD_START_FAST_ACQ;
      parsedCMD->funcPtr = StartFastACQ;
      result = getPar(0, PARAMETER_TIMEOUT, parsedCMD);
      break;
  	case CMD_STOP_ACQ:
      parsedCMD->cmd = CMD_STOP_ACQ;
      parsedCMD->funcPtr = StopACQ;
      result = getPar(0, PARAMETER_TIMEOUT, parsedCMD);
      break;
  	case CMD_SET_SAMPLE_PERIOD:
      parsedCMD->cmd = CMD_SET_SAMPLE_PERIOD;
      parsedCMD->funcPtr = SetSamplePeriod;
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD);
      break;
  	case CMD_SET_AVERAGE_COUNT:
      parsedCMD->cmd =  CMD_SET_AVERAGE_COUNT;
      parsedCMD->funcPtr =  SetAverageCount;
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD);
      break;
  	case CMD_SET_MEASURMENT_COUNT:
      parsedCMD->cmd =  CMD_SET_MEASURMENT_COUNT;
      parsedCMD->funcPtr =  SetMeasurmentCount;
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD);
      break;
  	case CMD_SET_SEQUENCER:
      parsedCMD->cmd =  CMD_SET_SEQUENCER;
      parsedCMD->funcPtr = SetSequencer;
      result = getPar(4, PARAMETER_TIMEOUT, parsedCMD);
      break;
  	case CMD_SET_DAC_VALUE:
      parsedCMD->cmd =  CMD_SET_DAC_VALUE;
      parsedCMD->funcPtr = SetDACvalue;
      result = getPar(2, PARAMETER_TIMEOUT, parsedCMD);
      break;
  	default:
      parsedCMD->cmd = 0;
      parsedCMD->funcPtr = NULL;
      for(uint8_t i = 0; i < MAX_PARAMETER_COUNT; i++)  parsedCMD->par[i] = 0;
      result = FALSE;
      break;
	}
  return result;	//return result
}

bool getPar(uint8_t parCount, uint16_t timeout, CMD_t *CMDpar){
  bool result = FALSE;	//set result to false
  uint8_t currentPar = 0; //index of current parameter
  uint8_t newChar = 0, prevChar = 0;
  for(uint8_t i = 0; i < MAX_PARAMETER_COUNT; i++)  CMDpar->par[i] = 0; //set all parameters to 0

  while(!result && timeout){  //wait for complete parameters or timeout
    if(udi_cdc_get_nb_received_data()){ //was new char received?
      newChar = udi_cdc_getc(); //get the char
      if(newChar == '\r') //end of command
      {
        if(parCount == (currentPar + 1) || parCount == 0)  result = TRUE;  //were all parameters received?
        else result = FALSE;
      }
      else if(newChar == ',') //new parameter
      {
        currentPar++;
      }
      else if(newChar == '\b')  //backspace remove previous char
      {
        if (prevChar == ',' && currentPar != 0)  currentPar--;
        else  CMDpar->par[currentPar] /= 10;
      }
      else if(newChar >= '0' && newChar <= '9') //got parameter
      {
        CMDpar->par[currentPar] *= 10;
        CMDpar->par[currentPar] += newChar - '0';
      }
      prevChar = newChar; //save char in case of backspace
      newChar = 0; //reset char
    }
    else //otherwise wait for 1ms before checking again
    {
      //delay(1);
      //timeout--;
    }
  }
  
  return result;	//return result
}


bool StartACQ (uint16_t *parPtr, daq_settings_t *settings)
{
  bool result = TRUE;	//set result to false
  
  settings->startAcq = 1;
  settings->binMode = ASCII_MODE;
  //aquisition_start();
  char buf[50];
  uint8_t len = sprintf(buf, "Acquisition started in ASCII\n\r");
  udi_cdc_write_buf(buf, len);
  
  return result;	//return result
}

bool StartFastACQ (uint16_t *parPtr, daq_settings_t *settings)
{
  bool result = TRUE;	//set result to false
  
  settings->startAcq = 1;
  settings->binMode = FAST_MODE;
  //aquisition_start();
  char buf[50];
  uint8_t len = sprintf(buf, "Acquisition started in binary\n\r");
  udi_cdc_write_buf(buf, len);
  
  return result;	//return result
}

bool StopACQ (uint16_t *parPtr, daq_settings_t *settings)
{
  bool result = TRUE;	//set result to false
  
  settings->startAcq = 0;
  //aquisition_stop();
  char buf[50];
  uint8_t len = sprintf(buf, "Acquisition stopped\n\r");
  udi_cdc_write_buf(buf, len);
  
  return result;	//return result
}

bool SetSamplePeriod (uint16_t *parPtr, daq_settings_t *settings)
{
  bool result = TRUE;	//set result to false
  
  settings->acqusitionTime = *parPtr;
  char buf[50];
  uint8_t len = sprintf(buf, "Sample period set to %u uS\n\r", settings->acqusitionTime);
  udi_cdc_write_buf(buf, len);
  
  return result;	//return result
}

bool SetAverageCount (uint16_t *parPtr, daq_settings_t *settings)
{
  bool result = TRUE;	//set result to false
  
  settings->averaging = *parPtr;
  char buf[50];
  uint8_t len = sprintf(buf, "DAQ will attempt to take %u samples per channel\n\r", settings->averaging);
  udi_cdc_write_buf(buf, len);
  //todo: limit samples per channel
  
  return result;	//return result
}

bool SetMeasurmentCount (uint16_t *parPtr, daq_settings_t *settings)
{
  bool result = TRUE;	//set result to false
  
  settings->acquisitionNbr =  *parPtr;
  char buf[50];
  uint8_t len = sprintf(buf, "DAQ will sample all enabled channels %u times\n\r", settings->acquisitionNbr);
  udi_cdc_write_buf(buf, len);
  //todo: limit samples per channel
  
  return result;	//return result
}

bool SetSequencer (uint16_t *parPtr, daq_settings_t *settings)
{
  bool result = TRUE;	//set result to false
  
  for(uint8_t i = 0; i < 4; i++)  settings->sequence[i] = *(parPtr + i);
  char buf[50];
  uint8_t len = sprintf(buf, "Sequence set to: %d, %d, %d, %d\n\r", *(parPtr+0), *(parPtr+1), *(parPtr+2), *(parPtr+3));
  udi_cdc_write_buf(buf, len);
  
  return result;	//return result
}

bool SetDACvalue (uint16_t *parPtr, daq_settings_t *settings)
{
  bool result = TRUE;	//set result to false
  
  settings->DACval[*parPtr] = *(parPtr + 1);
  char buf[50];
  uint8_t len = sprintf(buf, "DAC channel %u set to %u mV\n\r", *(parPtr + 0), *(parPtr + 1));
  udi_cdc_write_buf(buf, len);
  
  return result;	//return result
}
