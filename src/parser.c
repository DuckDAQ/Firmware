/************************************************************************************//**
* \file     parser.c
* \brief    This module implements parser for DAQ commands.
****************************************************************************************/ 

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdio.h>
#include "parser.h"


/************************************************************************************//**
** \brief     Submits a message to parser.
** \param     Command to parse, pointer to command structure, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool parseCommand (uint8_t CMD, CMD_t *parsedCMD, COM_t *comInterface)
{
  /* Set result to false. */
	bool result = FALSE;
	/* Is command supported? */
	switch (CMD)
	{
  	case CMD_START_ACQ:
      /* Save command if needed later */
      parsedCMD->cmd = CMD_START_ACQ;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = StartACQ;
      /* wait for termination of line */
      result = getPar(0, PARAMETER_TIMEOUT, NULL, comInterface);
      break;
  	case CMD_START_FAST_ACQ:
      /* Save command if needed later */
      parsedCMD->cmd = CMD_START_FAST_ACQ;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = StartFastACQ;
      /* wait for termination of line */
      result = getPar(0, PARAMETER_TIMEOUT, NULL, comInterface);
      break;
  	case CMD_STOP_ACQ:
      /* Save command if needed later */
      parsedCMD->cmd = CMD_STOP_ACQ;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = StopACQ;
      result = getPar(0, PARAMETER_TIMEOUT, NULL, comInterface);
      break;
  	case CMD_SET_SAMPLE_PERIOD:
      /* Save command if needed later */
      parsedCMD->cmd = CMD_SET_SAMPLE_PERIOD;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = SetSamplePeriod;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
  	case CMD_SET_AVERAGE_COUNT:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_AVERAGE_COUNT;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr =  SetAverageCount;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
  	case CMD_SET_MEASURMENT_COUNT:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_MEASURMENT_COUNT;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr =  SetMeasurmentCount;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
  	case CMD_SET_SEQUENCER:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_SEQUENCER;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = SetSequencer;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(4, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
  	case CMD_SET_DAC_VALUE:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_DAC_VALUE;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = SetDACvalue;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(2, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    /* Command not supported */
  	default:
      /* Reset command */
      parsedCMD->cmd = 0;
      /* Pointer not available */
      parsedCMD->funcPtr = NULL;
      break;
	}
  /* Return result */
  return result;
} /*** end of parseCommand ***/


/************************************************************************************//**
** \brief     Get parameters and wait for termination of line.
** \param     Number of required parameters, timeout, pointer to par array in CMD
**            structure, pointer to interface module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool getPar(uint8_t parCount, uint16_t timeout, int32_t *parPtr, COM_t *comInterface)
{
  /* Set result to false. */
  bool result = FALSE;
  /* Receive parameter buffer */
  uint8_t buf[MAX_PARAMETER_COUNT][MAX_PARAMETER_LENGHT];
  uint8_t newChar = 0, currentPar = 0, prevIdx = 0, idx = 0;
  
  /* Wait for complete parameters or timeout. */
  while(!result && timeout)
  {
    /* Was new char received? */
    if(comInterface->available())
    {
      /* Get the char */
      newChar = comInterface->read();
      /* FSM */
      /* End of command */
      if(newChar == '\r')
      {
        /* Were all parameters received? */
        if((parCount == (currentPar + 1) && idx) || !parCount)
        {
          /* Convert all parameters to int */
          for(uint8_t i = 0; i < parCount; i++) *(parPtr + i) = (int32_t)atoi((char*)buf[i]);
          /* All parameters received */
          result = TRUE;
        }
        /* Parameters not received */     
        else timeout = 0;
      }
      /* New parameter */
      else if(newChar == ',')
      {
        /* Go to next parameter */
        currentPar++;
        /* Set index of new parameter buffer to 0. */
        idx = 0;
      }
      /* Backspace, remove previous char */
      else if(newChar == '\b')
      {
        /* Go to previous parameter */
        if (buf[currentPar][idx - 1] == ',' && currentPar)
        {
          /* Go to previous parameter */
          currentPar--;
          /* Go to new index in previous parameter. */
          idx = prevIdx + 1;
        }
        /* Go to previous char */  
        else if(idx) idx--;
        /* ERROR: first parameter and index. */
        else timeout = 0;
        /* Set previous parameter to 0.  */
        buf[currentPar][idx] = 0;
      }
      /* Got parameter */
      else if((newChar >= '0' && newChar <= '9') || newChar == '-')
      {
        /* Save new char to parameter buffer. */
        buf[currentPar][idx] = newChar;
        /* Go to new char in parameter buffer */
        idx++;
      }
      /* Save idx in case of backspace. */
      prevIdx = idx;
      /* Reset char */
      newChar = 0;
    }
    /* Otherwise wait for 1ms before checking again. */
    else
    {
      //delay(1);
      //timeout--;
    }
  }
  /* Return result */
  return result;
} /*** end of getPar ***/


/************************************************************************************//**
** \brief     Start acquisition
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool StartACQ (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Set result to false. */
  bool result = FALSE;
  
  /* Set parameter */
  settings->binMode = (uint8_t)ASCII_MODE;
  /* Configure core with new settings. */
  core_configure(settings);
  /* Start core */
  //core_start();
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf, "Acquisition started in ASCII\n\r");
  /* Set result */
  result = TRUE;
  
  /* Return result */
  return result;
} /*** end of StartACQ ***/


/************************************************************************************//**
** \brief     Start acquisition in binary
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool StartFastACQ (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Set result to false. */
  bool result = FALSE;
  
  /* Set parameter */
  settings->binMode = (uint8_t)FAST_MODE;
  /* Configure core with new settings. */
  core_configure(settings); //configure core with new settings
  /* Start core */
  //core_start();
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf, "Acquisition started in binary\n\r");
  /* Set result */
  result = TRUE;
  
  /* Return result */
  return result;
} /*** end of StartFastACQ ***/


/************************************************************************************//**
** \brief     Stop acquisition
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool StopACQ (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Set result to false. */
  bool result = FALSE;
  
  /* Configure core with new settings. */
  core_configure(settings);
  /* Stop core */
  //TODO: call function to stop core
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf, "Acquisition stopped\n\r");
  /* Set result */
  result = TRUE;
  
  /* Return result */
  return result;
} /*** end of StopACQ ***/


/************************************************************************************//**
** \brief     Set sample period
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SetSamplePeriod (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Set result to false. */
  bool result = FALSE;
  
  /* Is parameter is range? */
  if(*parPtr >= SAMPLE_PERIOD_LOWRANGE && *parPtr <= SAMPLE_PERIOD_HIGHRANGE)
  {
    /* Set parameter */
    settings->acqusitionTime = (uint16_t)*parPtr;
    /* Configure core with new settings. */
    core_configure(settings);
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf, "Sample period set to %u uS\n\r",
                                settings->acqusitionTime);
    /* Set result */
    result = TRUE;
  }
  
  /* Return result */
  return result;
} /*** end of SetSamplePeriod ***/


/************************************************************************************//**
** \brief     Set averaging
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SetAverageCount (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Set result to false. */
  bool result = FALSE;
  
  /* Is parameter is range? */
  if(*parPtr >= AVERAGE_COUNT_LOWRANGE &&  *parPtr <= AVERAGE_COUNT_HIGHRANGE)
  {
    /* Set parameter */
    settings->averaging = (uint16_t)*parPtr;
    /* Configure core with new settings. */
    core_configure(settings);
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf,
                                "DAQ will attempt to take %u samples per channel\n\r",
                                settings->averaging);
    /* Set result */
    result = TRUE;
  }
  
  /* Return result */
  return result;
} /*** end of SetAverageCount ***/


/************************************************************************************//**
** \brief     Set number of samples
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SetMeasurmentCount (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Set result to false. */
  bool result = FALSE;
  
  /* Is parameter is range? */
  if(*parPtr >= MEASURMENT_COUNT_LOWRANGE &&  *parPtr <= MEASURMENT_COUNT_HIGHRANGE)
  {
    /* Set parameter */
    settings->acquisitionNbr = (uint16_t)*parPtr;
    /* Configure core with new settings. */
    core_configure(settings);
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf,
                                "DAQ will sample all enabled channels %u times\n\r",
                                settings->acquisitionNbr);
    /* Set result */
    result = TRUE;
  }
  
  /* Return result */
  return result;
} /*** end of SetMeasurmentCount ***/


/************************************************************************************//**
** \brief     Enable channels and set channel sequence
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SetSequencer (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Set result to false. */
  bool result = FALSE;
  
  /* Is parameter is range? */
  if((*(parPtr + 0) >= SEQUENCER_LOWRANGE && *(parPtr + 0) <= SEQUENCER_HIGHRANGE) &&
     (*(parPtr + 1) >= SEQUENCER_LOWRANGE && *(parPtr + 1) <= SEQUENCER_HIGHRANGE) &&
     (*(parPtr + 2) >= SEQUENCER_LOWRANGE && *(parPtr + 2) <= SEQUENCER_HIGHRANGE) &&
     (*(parPtr + 3) >= SEQUENCER_LOWRANGE && *(parPtr + 3) <= SEQUENCER_HIGHRANGE))
  {
    /* Set parameters */
    for(uint8_t i = 0; i < 4; i++)  settings->sequence[i] = (uint8_t)*(parPtr + i);
    /* Configure core with new settings. */
    core_configure(settings);
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf,
                                "Sequence set to: %u, %u, %u, %u\n\r",
                                settings->sequence[0], settings->sequence[1],
                                settings->sequence[2], settings->sequence[3]);
    /* Set result */
    result = TRUE;
  }  
  
  /* Return result */
  return result;
} /*** end of SetSequencer ***/


/************************************************************************************//**
** \brief     Set DAC output value
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SetDACvalue (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Set result to false. */
  bool result = FALSE;
  
  /* Is parameter is range? */
  if((*(parPtr + 0) >= DAC_VALUE_PAR0_LOWRANGE &&  *(parPtr + 0) <= DAC_VALUE_PAR0_HIGHRANGE) &&
     (*(parPtr + 1) >= DAC_VALUE_PAR1_LOWRANGE &&  *(parPtr + 1) <= DAC_VALUE_PAR1_HIGHRANGE))
  {
    /* Set parameter */
    settings->DACval[(uint8_t)*parPtr] = (int16_t)*(parPtr + 1);
    settings->DACflag=1;
    /* Configure DAC with new values. */
    //DacSetVal(&settings);
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf, "DAC channel %u set to %d mV\n\r",
                                (uint8_t)*(parPtr + 0), settings->DACval[(uint8_t)*parPtr]);
    /* Set result */
    result = TRUE;
  }
  
  /* Return result */
  return result;
} /*** end of SetDACvalue ***/

/*********************************** end of parser.c ***********************************/