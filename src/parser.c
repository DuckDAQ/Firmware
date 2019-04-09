/*
 * parser.c
 *
 * Created: 4.3.2015 12:29:30
 *  Author: Matjaz
 */ 

#include <asf.h>
#include <stdio.h>
#include "parser.h"


bool parseCommand (uint8_t CMD, CMD_t *parsedCMD) //parse received command
{
	bool result = FALSE;	//set result to false
	
	switch (CMD)  //is command supported?
	{
  	case CMD_START_ACQ:
      parsedCMD->cmd = CMD_START_ACQ; //save command
      parsedCMD->funcPtr = StartACQ;  //set pointer to function
      result = getPar(0, PARAMETER_TIMEOUT, NULL);  //wait for termination of line
      break;
  	case CMD_START_FAST_ACQ:
      parsedCMD->cmd = CMD_START_FAST_ACQ; //save command
      parsedCMD->funcPtr = StartFastACQ;  //set pointer to function
      result = getPar(0, PARAMETER_TIMEOUT, NULL);  //wait for termination of line
      break;
  	case CMD_STOP_ACQ:
      parsedCMD->cmd = CMD_STOP_ACQ; //save command
      parsedCMD->funcPtr = StopACQ;  //set pointer to function
      result = getPar(0, PARAMETER_TIMEOUT, NULL);  //wait for termination of line
      break;
  	case CMD_SET_SAMPLE_PERIOD:
      parsedCMD->cmd = CMD_SET_SAMPLE_PERIOD; //save command
      parsedCMD->funcPtr = SetSamplePeriod;  //set pointer to function
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par);  //get parameters for command or wait for termination of line
      break;
  	case CMD_SET_AVERAGE_COUNT:
      parsedCMD->cmd =  CMD_SET_AVERAGE_COUNT; //save command
      parsedCMD->funcPtr =  SetAverageCount;  //set pointer to function
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par);  //get parameters for command or wait for termination of line
      break;
  	case CMD_SET_MEASURMENT_COUNT:
      parsedCMD->cmd =  CMD_SET_MEASURMENT_COUNT; //save command
      parsedCMD->funcPtr =  SetMeasurmentCount;  //set pointer to function
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par);  //get parameters for command or wait for termination of line
      break;
  	case CMD_SET_SEQUENCER:
      parsedCMD->cmd =  CMD_SET_SEQUENCER; //save command
      parsedCMD->funcPtr = SetSequencer;  //set pointer to function
      result = getPar(4, PARAMETER_TIMEOUT, parsedCMD->par);  //get parameters for command or wait for termination of line
      break;
  	case CMD_SET_DAC_VALUE:
      parsedCMD->cmd =  CMD_SET_DAC_VALUE; //save command
      parsedCMD->funcPtr = SetDACvalue;  //set pointer to function
      result = getPar(2, PARAMETER_TIMEOUT, parsedCMD->par);  //get parameters for command or wait for termination of line
      break;
  	default:  //command not supported
      parsedCMD->cmd = 0; //reset command
      parsedCMD->funcPtr = NULL;  //pointer not available
      break;
	}
  return result;	//return result
}


bool getPar(uint8_t parCount, uint16_t timeout, int32_t *parPtr)  //get parameters or wait for termination of line
{
  bool result = FALSE;	//set result to false
  uint8_t buf[MAX_PARAMETER_COUNT][MAX_PARAMETER_LENGHT]; //receive parameter buffer
  uint8_t newChar = 0, prevChar = 0, currentPar = 0, prevIdx = 0, idx = 0;

  while(!result && timeout){  //wait for complete parameters or timeout
    if(udi_cdc_get_nb_received_data()){ //was new char received?
      newChar = udi_cdc_getc(); //get the char
      if(newChar == '\r') //end of command
      {
        if((parCount == (currentPar + 1) && idx) || parCount)  //were all parameters received?
        {
          for(uint8_t i = 0; i < parCount; i++) *(parPtr + i) = (int32_t)atoi((char*)buf[i]); //convert all parameters to int
          result = TRUE;  //all parameters received
        }        
        else timeout = 0; //parameters not received
      }
      else if(newChar == ',') //new parameter
      {
        currentPar++; //go to next parameter
        idx = 0;  //set index ox new parameter buffer to 0
      }
      else if(newChar == '\b')  //backspace remove previous char
      {
        if (prevChar == ',' && currentPar)  //go to previous parameter
        {
          currentPar--; //go to previous parameter
          idx = prevIdx + 1;  //go to new index in previous parameter
        }        
        else if(idx) idx--; //go to previous char
        else timeout = 0; //ERROR: first parameter and index
        buf[currentPar][idx] = 0; //set previous parameter to 0 
      }
      else if((newChar >= '0' && newChar <= '9') || newChar == '-') //got parameter
      {
        buf[currentPar][idx] = newChar; //save new char to parameter buffer
        idx++; //go to new char in parameter buffer
      }
      prevChar = newChar; //save char in case of backspace
      prevIdx = idx;  //save idx in case of backspace
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


bool StartACQ (int32_t *parPtr, daq_settings_t *settings) //start acquisition
{
  bool result = FALSE;	//set result to false
  
  settings->startAcq = (uint8_t)1;  //set parameter
  settings->binMode = (uint8_t)ASCII_MODE;  //set parameter
  //if(acquisition_start())
  //{
    //print msg to inform user
    char buf[50];
    uint8_t len = sprintf(buf, "Acquisition started in ASCII\n\r");
    udi_cdc_write_buf(buf, len);
    result = TRUE;
  //}
  
  return result;	//return result
}

bool StartFastACQ (int32_t *parPtr, daq_settings_t *settings) //start acquisition in binary
{
  bool result = FALSE;	//set result to false
  
  settings->startAcq = (uint8_t)1;  //set parameter
  settings->binMode = (uint8_t)FAST_MODE;  //set parameter
  //if(aquisition_start())
  //{
    //print msg to inform user
    char buf[50];
    uint8_t len = sprintf(buf, "Acquisition started in binary\n\r");
    udi_cdc_write_buf(buf, len);
    result = TRUE;
  //}
  
  return result;	//return result
}

bool StopACQ (int32_t *parPtr, daq_settings_t *settings)  //stop acquisition
{
  bool result = FALSE;	//set result to false
  
  settings->startAcq = (uint8_t)0;  //set parameter
  //if(aquisition_stop())
  //{
    //print msg to inform user
    char buf[50];
    uint8_t len = sprintf(buf, "Acquisition stopped\n\r");
    udi_cdc_write_buf(buf, len);
    result = TRUE;
  //}
  
  return result;	//return result
}

bool SetSamplePeriod (int32_t *parPtr, daq_settings_t *settings)  //set sample period
{
  bool result = FALSE;	//set result to false
  
  if(*parPtr >= SAMPLE_PERIOD_LOWRANGE && *parPtr <= SAMPLE_PERIOD_HIGHRANGE) //is parameter is range?
  {
    settings->acqusitionTime = (uint16_t)*parPtr;  //set parameter
    //print msg to inform user
    char buf[50];
    uint8_t len = sprintf(buf, "Sample period set to %u uS\n\r", settings->acqusitionTime);
    udi_cdc_write_buf(buf, len);
    result = TRUE;
  }
  
  return result;	//return result
}

bool SetAverageCount (int32_t *parPtr, daq_settings_t *settings)  //set averaging
{
  bool result = FALSE;	//set result to false
  
  if(*parPtr >= AVERAGE_COUNT_LOWRANGE &&  *parPtr <= AVERAGE_COUNT_HIGHRANGE) //is parameter is range?
  {
    settings->averaging = (uint16_t)*parPtr;  //set parameter
    //print msg to inform user
    char buf[50];
    uint8_t len = sprintf(buf, "DAQ will attempt to take %u samples per channel\n\r", settings->averaging);
    udi_cdc_write_buf(buf, len);
    result = TRUE;
  }
  
  return result;	//return result
}

bool SetMeasurmentCount (int32_t *parPtr, daq_settings_t *settings) //set number of samples
{
  bool result = FALSE;	//set result to false
  
  if(*parPtr >= MEASURMENT_COUNT_LOWRANGE &&  *parPtr <= MEASURMENT_COUNT_HIGHRANGE) //is parameter is range?
  {
    settings->acquisitionNbr =  (uint16_t)*parPtr;  //set parameter
    //print msg to inform user
    char buf[50];
    uint8_t len = sprintf(buf, "DAQ will sample all enabled channels %u times\n\r", settings->acquisitionNbr);
    udi_cdc_write_buf(buf, len);
    result = TRUE;
  }
  
  return result;	//return result
}

bool SetSequencer (int32_t *parPtr, daq_settings_t *settings) //enable channels and set channel sequence
{
  bool result = FALSE;	//set result to false
  
  if((*(parPtr + 0) >= SEQUENCER_LOWRANGE && *(parPtr + 0) <= SEQUENCER_HIGHRANGE) &&
     (*(parPtr + 1) >= SEQUENCER_LOWRANGE && *(parPtr + 1) <= SEQUENCER_HIGHRANGE) &&
     (*(parPtr + 2) >= SEQUENCER_LOWRANGE && *(parPtr + 2) <= SEQUENCER_HIGHRANGE) &&
     (*(parPtr + 3) >= SEQUENCER_LOWRANGE && *(parPtr + 3) <= SEQUENCER_HIGHRANGE)) //are parameters is range?
  {
    for(uint8_t i = 0; i < 4; i++)  settings->sequence[i] = (uint8_t)*(parPtr + i);  //set parameters
    //print msg to inform user
    char buf[50];
    uint8_t len = sprintf(buf, "Sequence set to: %u, %u, %u, %u\n\r", settings->sequence[0], settings->sequence[1], settings->sequence[2], settings->sequence[3]);
    udi_cdc_write_buf(buf, len);
    result = TRUE;
  }  
  
  return result;	//return result
}

bool SetDACvalue (int32_t *parPtr, daq_settings_t *settings)  //set DAC output value
{
  bool result = FALSE;	//set result to false
  
  if((*(parPtr + 0) >= DAC_VALUE_PAR0_LOWRANGE &&  *(parPtr + 0) <= DAC_VALUE_PAR0_HIGHRANGE) &&
     (*(parPtr + 1) >= DAC_VALUE_PAR1_LOWRANGE &&  *(parPtr + 1) <= DAC_VALUE_PAR1_HIGHRANGE))  //are parameters is range?
  {
    settings->DACval[(uint8_t)*parPtr] = (int16_t)*(parPtr + 1);  //set parameter
    //print msg to inform user
    char buf[50];
    uint8_t len = sprintf(buf, "DAC channel %u set to %d mV\n\r", (uint8_t)*(parPtr + 0), settings->DACval[(uint8_t)*parPtr]);
    udi_cdc_write_buf(buf, len);
    result = TRUE;
  }
  
  return result;	//return result
}
