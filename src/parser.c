/************************************************************************************//**
* \file     parser.c
* \brief    This module implements parser for DAQ commands.
****************************************************************************************/ 

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdio.h>                   /* Standard I/O functions.                        */
#include "parser.h"                  /* Protocol parser with command handlers          */


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* Parser functions */
bool getPar(uint8_t parCount, uint16_t timeout, int32_t *parPtr, COM_t *comInterface);
/* Command handlers */
bool startACQ (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool stopACQ (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setMode (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setSamplePeriod (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setAverageCount (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setMeasurmentCount (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setSequencer (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setADCgain (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setADClowRes (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setBlockSize (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool SetLutValues (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool DacSetNumberOfRepeats(int32_t *parPtr,daq_settings_t *settings,COM_t *comInterface);
bool DacSetFreq (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool DacSetLutLength (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool DacStop (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool DacStart (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool DacTransfer (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool GetLutCounter(int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);


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
      parsedCMD->funcPtr = startACQ;
      /* wait for termination of line */
      result = getPar(0, PARAMETER_TIMEOUT, NULL, comInterface);
      break;
    
    case CMD_STOP_ACQ:
      /* Save command if needed later */
      parsedCMD->cmd = CMD_STOP_ACQ;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = stopACQ;
      result = getPar(0, PARAMETER_TIMEOUT, NULL, comInterface);
      break;
    
    case CMD_SET_MODE:
      /* Save command if needed later */
      parsedCMD->cmd = CMD_SET_MODE;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = setMode;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_SET_SAMPLE_PERIOD:
      /* Save command if needed later */
      parsedCMD->cmd = CMD_SET_SAMPLE_PERIOD;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = setSamplePeriod;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_SET_AVERAGE_COUNT:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_AVERAGE_COUNT;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = setAverageCount;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_SET_MEASURMENT_COUNT:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_MEASURMENT_COUNT;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = setMeasurmentCount;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_SET_ADC_LOW_RESOLUTION:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_ADC_LOW_RESOLUTION;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = setADClowRes;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_SET_BLOCK_SIZE:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_BLOCK_SIZE;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = setBlockSize;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_SET_ADC_GAIN:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_ADC_GAIN;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = setADCgain;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(2, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_SET_SEQUENCER:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_SEQUENCER;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = setSequencer;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(4, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
      
    case CMD_SET_REPEAT_NUM:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_REPEAT_NUM;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = DacSetNumberOfRepeats;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_SET_DAC_FREQ: //we set period in microseconds, but ok
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_DAC_FREQ;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = DacSetFreq;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_SET_DAC_LUT:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_DAC_LUT;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = SetLutValues;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(2, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_DAC_STOP:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_DAC_STOP;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = DacStop;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(0, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_DAC_START:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_DAC_START;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = DacStart;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(0, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_DAC_LUT_LENGTH:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_DAC_LUT_LENGTH;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = DacSetLutLength;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_DAC_TRANSFER_MODE:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_DAC_TRANSFER_MODE;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = DacTransfer;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
      break;
    
    case CMD_DAC_LUT_COUNTER:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_DAC_LUT_COUNTER;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = GetLutCounter;
      /* Get parameters for command or wait for termination of line. */
      result = getPar(0, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
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
  for(uint8_t x = 0; x < MAX_PARAMETER_COUNT; x++)
  {
    for(uint8_t y = 0; y < MAX_PARAMETER_LENGHT; y++)
    {
      buf[x][y] = 0;
    }
  }
  uint8_t newChar = 0, currentPar = 0, prevIdx = 0, idx = 0;
  
  /* Wait for complete parameters or timeout. */
  while(!result && timeout)
  {
    /* Was new char received? */
    if(comInterface->available())
    {
      /* Get the char */
      newChar = comInterface->read();
      
      /* End of command */
      if(newChar == '\r')
      {
        /* Were all parameters received? */
        if((parCount == (currentPar + 1) && idx) || !parCount)
        {
          /* Convert all parameters to int */
          for(uint8_t i=0; i<parCount; i++) *(parPtr + i)=(int32_t)atoll((char*)buf[i]);
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
bool startACQ (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Start core */
  timerStart();
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf, "Acquisition started\n\r");
  /* Set result */
  return TRUE;
} /*** end of StartACQ ***/


/************************************************************************************//**
** \brief     Stop acquisition
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool stopACQ (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Stop core */
  timerStop();
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf, "Acquisition stopped\n\r");
  return TRUE;
} /*** end of StopACQ ***/


/************************************************************************************//**
** \brief     Start acquisition in binary
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool setMode (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Is parameter is range? */
  if(*parPtr >= MODE_LOWRANGE && *parPtr <= MODE_HIGHRANGE)
  {
    /* Set parameter */
    settings->mode = (uint8_t)*parPtr;
    /* Print msg to inform user */
    switch((uint8_t)*parPtr)
    {
      case ASCII_MODE:
        comInterface->len = sprintf((char*)comInterface->buf,
                                    "Acquisition started in ASCII\n\r");
        break;
      
      case BIN_MODE:
        comInterface->len = sprintf((char*)comInterface->buf,
                                    "Acquisition started in binary\n\r");
        break;
    }
    return TRUE;
  }

  return FALSE;
} /*** end of setMode ***/


/************************************************************************************//**
** \brief     Set sample period
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool setSamplePeriod (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Is parameter is range? */
  if(*parPtr >= SAMPLE_PERIOD_LOWRANGE && *parPtr <= SAMPLE_PERIOD_HIGHRANGE)
  {
    /* Set parameter */
    settings->acqusitionTime = (uint32_t)*parPtr;
    if(timerSetTimePeriod())
    {
      /* Print msg to inform user */
      comInterface->len = sprintf((char*)comInterface->buf,
                                  "Sample period set to %u us\n\r",
                                  settings->acqusitionTime);
      return TRUE;
    }
  }
  
  return FALSE;
} /*** end of SetSamplePeriod ***/


/************************************************************************************//**
** \brief     Set averaging
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool setAverageCount (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Is parameter is range? */
  if(*parPtr >= AVERAGE_COUNT_LOWRANGE &&  *parPtr <= AVERAGE_COUNT_HIGHRANGE)
  {
    /* Set parameter */
    settings->averaging = (uint16_t)*parPtr;
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf,
                                "DAQ will attempt to take %u samples per channel\n\r",
                                settings->averaging);
    return TRUE;
  }

  return FALSE;
} /*** end of SetAverageCount ***/


/************************************************************************************//**
** \brief     Set number of samples
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool setMeasurmentCount (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{  
  /* Is parameter is range? */
  if(*parPtr >= MEASURMENT_COUNT_LOWRANGE &&  *parPtr <= MEASURMENT_COUNT_HIGHRANGE)
  {
    /* Set parameter */
    settings->acquisitionNbr = (uint16_t)*parPtr;
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf,
                                "DAQ will sample all enabled channels %u times\n\r",
                                settings->acquisitionNbr);
    return TRUE;
  }

  return FALSE;
} /*** end of SetMeasurmentCount ***/


/************************************************************************************//**
** \brief     Enable channels and set channel sequence
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool setSequencer (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{

   /* Is parameter in range? */
  if((*(parPtr + 0) >= SEQUENCER_LOWRANGE && *(parPtr + 0) <= SEQUENCER_HIGHRANGE) &&
     (*(parPtr + 1) >= SEQUENCER_LOWRANGE && *(parPtr + 1) <= SEQUENCER_HIGHRANGE) &&
     (*(parPtr + 2) >= SEQUENCER_LOWRANGE && *(parPtr + 2) <= SEQUENCER_HIGHRANGE) &&
     (*(parPtr + 3) >= SEQUENCER_LOWRANGE && *(parPtr + 3) <= SEQUENCER_HIGHRANGE))
  {
    /* Set parameters */
     //TODO!
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf,
                                "Sequence set to: %u, %u, %u, %u\n\r",
                                settings->sequence[0], settings->sequence[1],
                                settings->sequence[2], settings->sequence[3]);
    return TRUE;
  }

  return FALSE;
} /*** end of SetSequencer ***/


/************************************************************************************//**
** \brief     Set ADC gain
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool setADCgain (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Is parameter is range? */
  if( ( (*(parPtr + 0) > 0) && (*(parPtr + 0) <= 4) ) &&
      ( (*(parPtr + 1) == ADC_GAIN_0_5) || (*(parPtr + 1) == ADC_GAIN_1) ||
        (*(parPtr + 1) == ADC_GAIN_2) ) )
  {
    /* Set parameter */
    settings->ADCgain[*(parPtr + 0) - 1] = (uint8_t)*(parPtr + 1);
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf,
                                "ADC channel %u gain set to %d\n\r",
                                (uint8_t)*(parPtr + 0), (int8_t)*(parPtr + 1));
    /* Call core handler */
    adcSetGain();
    /* Set result */
    return TRUE;
  }

  return FALSE;
} /*** end of SetDACvalue ***/


/************************************************************************************//**
** \brief     Set ADC resolution
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool setADClowRes (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Is parameter is range? */
  if( (*(parPtr + 0) == ADC_RES_12_BITS) || (*(parPtr + 0) == ADC_RES_10_BITS) )
  {
    /* Set parameter */
    settings->ADClowRes = (uint8_t)*(parPtr + 0);
    /* Print msg to inform user */
    if(settings->ADClowRes)
    {
      comInterface->len = sprintf((char*)comInterface->buf,
                                  "ADC resolution set to 10 bits.\n\r");
    }
    else
    {
      comInterface->len = sprintf((char*)comInterface->buf,
                                  "ADC resolution set to 12 bits.\n\r");
    }
    /* Call core handler */
    adcSetRes();
    /* Set result */
    return TRUE;
  }
  return FALSE;
} /*** end of setADClowRes ***/


/************************************************************************************//**
** \brief     Set block size
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool setBlockSize (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Is parameter is range? */
  if(*(parPtr + 0) >= BLOCK_SIZE_MIN &&  *(parPtr + 0) <= BLOCK_SIZE_MAX)
  {
    /* Set parameter */
    *(settings->blockSize) = (uint32_t)*(parPtr + 0);
    /* create new sync bytes for this blockSize */
    settings->syncBytes[0] = ((uint16_t)*(parPtr) & 0b1111111) | 0b10000000;
    settings->syncBytes[1] = (((uint16_t)*(parPtr)>>7) & 0b1111111) | 0b10000000;
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf, "Block size set to %u\n\r",
                                (uint16_t)*(parPtr + 0));
    /* Set result */
    return TRUE;
  }

  /* Return result */
  return FALSE;
} /*** end of setBlockSize ***/


/************************************************************************************//**
** \brief     SetLutValues
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool SetLutValues (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* 1. Parameter: LUT position */
  uint16_t location = (uint16_t)*(parPtr);
  /* 2. Parameter: LUT value */
  uint16_t value = (uint16_t)*(parPtr + 1);
  if( (location < 0) || (location > (DAC_LUT_LOCATION_HIGHRANGE * 2) + 1) ||
      (value < DAC_VALUE_MIN) || (value > DAC_VALUE_MAX) )
  {
    return false; //Parameter out of range
  }
  /* Set parameters */
  settings->Lut[location] = value;
  /* Print msg to inform user, $ is command for WinForms to send new Lut value */
  comInterface->len = sprintf((char*)comInterface->buf, "$");
  /* Return true */
  return true;
} /*** end of SetLutValues ***/


/************************************************************************************//**
** \brief     DacSetNumberOfRepeats
**            How many times will DAQ output LUT. -1 for continuous mode. Max 65500
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool DacSetNumberOfRepeats (int32_t *parPtr,daq_settings_t *settings,COM_t *comInterface)
{
  if(*parPtr < DAC_REPEAT_NUM_MIN || *parPtr > DAC_REPEAT_NUM_MAX)
  {
    /* Parameter out of range */
    return false;
  }
  /* Set parameters, 1. Parameter: LUT repeats */
  settings->NumOfRepeats = (uint16_t)*(parPtr);
  /* Reset current repeats */
  settings->CurrentRepeats = 0;
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf,
                              "LUT number of repeats was set to %u\n\r",
                              (uint16_t)*parPtr);
  /* Return true */
  return true;
} /*** end of DacSetNumberOfRepeats ***/


/************************************************************************************//**
** \brief     DacStart
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool DacStart (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Call core handler */
  dacTimerStart();
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf, "DAC timer started\n\r");
  /* Return true */
  return true;
} /*** end of DacStart ***/


/************************************************************************************//**
** \brief     DacStop
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool DacStop (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Call core handler */
  dacTimerStop();
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf, "DAC timer stopped\n\r");
  /* Return true */
  return true;
} /*** end of DacStop ***/


/************************************************************************************//**
** \brief     DacSetFreq
**            Timer period for DAC. With this period timer will be setting new values
**            to DAC (with PDC).
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool DacSetFreq (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  if(*parPtr < DAC_PERIOD_MIN || *parPtr > DAC_PERIOD_MAX)
  {
    /* Period mustn't be below 1(us) or above 65500(us). */
    return false;
  }
  /* Set parameters, 1. Parameter: DAC period (in us) */
  settings->DacPeriod = *parPtr;
  /* Call core handler */
  setDacPeriod();
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf, "Period set to: %u us\n\r",
                               settings->DacPeriod);
  /* Return true */
  return true;
} /*** end of DacSetFreq ***/


/************************************************************************************//**
** \brief     DacSetLutLength
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool DacSetLutLength (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  if(*(parPtr) < 1 || *(parPtr) > (DAC_LUT_LOCATION_HIGHRANGE+1)*2)
  {
    /* Parameter out of range, Mustn't be below 1 or above 2048 */
    return false;
  }
  /* Set parameters, 1. Parameter: LUT length */
  settings->LutLength = (uint16_t)*(parPtr);
  /* Call core handler */
  SetDacPdcLength();
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf,
                              "$LUT length of was set to %u\n\r", settings->LutLength);
  /* Return true */
  return true;
} /*** end of DacSetLutLength ***/


/************************************************************************************//**
** \brief     DacTransfer
**            0=HalfMode / 1=FullMode
**            Fullmode: 2 values at the same time to ADC FIFO buffer
**            Fullmode for both channels simultaniously
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool DacTransfer(int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  if(*parPtr < 0 || *parPtr > 1)
  {
    /* Parameter out of range */
    return FALSE;
  }
  /* Set parameters, 1. Parameter: Transfer mode. */
  setDacTransferMode(*parPtr);
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf,
                              "Dac transfer mode set to %u\n\r", (uint8_t)*parPtr);
  /* Return true */
  return true;
} /*** end of DacTransfer ***/


/************************************************************************************//**
** \brief     GetLutCounter
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool GetLutCounter(int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf, "#%u\n\r", GetLutCntr());
  /* Return true */
  return true;
} /*** end of GetLutCounter ***/


/*********************************** end of parser.c ***********************************/