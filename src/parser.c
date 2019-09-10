/************************************************************************************//**
* \file     parser.c
* \brief    This module implements parser for DAQ commands.
****************************************************************************************/ 

/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdio.h>
#include "parser.h"


/***************************************************************************************
* Function prototypes
****************************************************************************************/
/* Parser functions */
bool getPar(uint8_t parCount, uint16_t timeout, int32_t *parPtr, COM_t *comInterface);
/* Parameter handlers */
bool startACQ (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool stopACQ (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setMode (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setSamplePeriod (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setAverageCount (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setMeasurmentCount (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setSequencer (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setDACvalue (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setADCgain (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setADClowRes (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool setBlockSize (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);

bool SetLutValues (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool DacSetNumberOfRepeats (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool DacSetFreq (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool DacSetLutLength (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool DacStop (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool DacStart (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);
bool DacSequencer(int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface);


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
    case CMD_SET_DAC_VALUE:
      /* Save command if needed later */
      parsedCMD->cmd =  CMD_SET_DAC_VALUE;
      /* Set pointer to the command handler */
      parsedCMD->funcPtr = setDACvalue;
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
		result = getPar(2, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
		break;
	case CMD_SET_DAC_FREQ:
		/* Save command if needed later */
		parsedCMD->cmd =  CMD_SET_DAC_FREQ;
		/* Set pointer to the command handler */
		parsedCMD->funcPtr = DacSetFreq;
		/* Get parameters for command or wait for termination of line. */
		result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
		break;
	case CMD_SET_DAC_SEQUENCER:
		/* Save command if needed later */
		parsedCMD->cmd =  CMD_SET_DAC_SEQUENCER;
		/* Set pointer to the command handler */
		parsedCMD->funcPtr = DacSequencer;
		/* Get parameters for command or wait for termination of line. */
		result = getPar(2, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
		break;
	case CMD_SET_DAC_LUT:
		/* Save command if needed later */
		parsedCMD->cmd =  CMD_SET_DAC_LUT;
		/* Set pointer to the command handler */
		parsedCMD->funcPtr = SetLutValues;
		/* Get parameters for command or wait for termination of line. */
		result = getPar(3, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
		break;
	case CMD_DAC_STOP:
		/* Save command if needed later */
		parsedCMD->cmd =  CMD_DAC_STOP;
		/* Set pointer to the command handler */
		parsedCMD->funcPtr = DacStop;
		/* Get parameters for command or wait for termination of line. */
		result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
		break;
	case CMD_DAC_START:
		/* Save command if needed later */
		parsedCMD->cmd =  CMD_DAC_START;
		/* Set pointer to the command handler */
		parsedCMD->funcPtr = DacStart;
		/* Get parameters for command or wait for termination of line. */
		result = getPar(1, PARAMETER_TIMEOUT, parsedCMD->par, comInterface);
		break;
	case CMD_DAC_LUT_LENGTH:
		/* Save command if needed later */
		parsedCMD->cmd =  CMD_DAC_LUT_LENGTH;
		/* Set pointer to the command handler */
		parsedCMD->funcPtr = DacSetLutLength;
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
** \brief     Set DAC output value
** \param     Pointer to par array in CMD, pointer to daq settings, pointer to interface
**            module.
** \return    True if successful, false otherwise.
**
****************************************************************************************/
bool setDACvalue (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
  /* Is parameter is range? */
  if( (*(parPtr + 0) >= DAC_CH_LOWRANGE && *(parPtr + 0) <= DAC_CH_HIGHRANGE) &&
      (*(parPtr + 1) >= DAC_VALUE_PAR1_LOWRANGE && *(parPtr + 1) <= DAC_VALUE_PAR1_HIGHRANGE) )
  {
    /* Add 10000 to remove negative values */
    uint32_t tmp = (uint32_t)((int16_t)*(parPtr + 1) + 10000);
    /* Convert mV to 0...4095 */
    tmp *= 25;
    tmp += 36;
    tmp *= 4095;
    tmp /= 500000;
    /* Set parameter */
    settings->DACval[((uint8_t)*parPtr) - 1] = (uint16_t)tmp;
    coreSetDacVal();
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf,
                                "DAC channel %u set to %d mV\n\r",
                                (uint8_t)*(parPtr + 0), (int16_t)*(parPtr + 1));
    /* Set result */
    return TRUE;
  }

  return FALSE;
} /*** end of SetDACvalue ***/


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
				//create new sync bytes for this blockSize
				settings->syncBytes[0] = ((uint16_t)*(parPtr) & 0b1111111) | 0b10000000;
				settings->syncBytes[1] = (((uint16_t)*(parPtr)>>7) & 0b1111111) | 0b10000000;
    /* Print msg to inform user */
    comInterface->len = sprintf((char*)comInterface->buf, "Block size set to %u\n\r",
                                (uint16_t)*(parPtr + 0));
    /* Call core handler */
    //pdcChangeAdcBlockSize();
    /* Set result */
    return TRUE;
  }

  /* Return result */
  return FALSE;
} /*** end of setBlockSize ***/


bool SetLutValues (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
				//1. Parameter: DAC channel
				//2. Parameter: LUT position
				//3. Parameter: LUT value
				uint8_t channel = (uint8_t)*(parPtr + 0);
				uint16_t location = (uint16_t)*(parPtr + 1);
				uint16_t value = (uint16_t)*(parPtr + 2);
				if(channel < DAC_CH_LOWRANGE || channel > DAC_CH_HIGHRANGE
				|| location < DAC_LUT_LOCATION_LOWRANGE || location > DAC_LUT_LOCATION_HIGHRANGE
				|| value < DAC_VALUE_MIN || value > DAC_VALUE_MAX
				) return false; //Parameter out of range
				
				/* Set parameters */
				settings->DAC[channel-1].Lut[location] = value;
				/* Print msg to inform user */
				comInterface->len = sprintf((char*)comInterface->buf,
				"$"); //$ is command for WinForms to send new Lut value
				return true;
}
//How many times will DAQ output LUT. -1 for continuous mode. Max 65500
bool DacSetNumberOfRepeats (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
			//1. Parameter: DAC channel
			//2. Parameter: LUT repeats
			if(*(parPtr + 0) < DAC_CH_LOWRANGE || *(parPtr + 0) > DAC_CH_HIGHRANGE
			|| *(parPtr + 1) < DAC_REPEAT_NUM_MIN || *(parPtr + 1) > DAC_REPEAT_NUM_MAX
			) return false; //Parameter out of range
			
			/* Set parameters */
			settings->DAC[(uint16_t)*parPtr].NumOfRepeats = (uint16_t)*(parPtr + 1);
			/* Print msg to inform user */
			comInterface->len = sprintf((char*)comInterface->buf,
			"LUT repeats on DAC channel %u was set to %u\n\r",
			(uint16_t)*parPtr,
			settings->DAC[(uint16_t)(*parPtr)-1].NumOfRepeats);
			return true;
}

bool DacStop (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{

}

bool DacStart (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{

}

bool DacSequencer (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
		//1. Parameter: 1. DAC channel
  //2. Parameter: 2. DAC channel
  if(*(parPtr + 0) < 0 || *(parPtr + 0) > DAC_CH_HIGHRANGE
  || *(parPtr + 1) < 0 || *(parPtr + 1) > DAC_CH_HIGHRANGE //Has to be above 0 and lower 1025
  ) return false; //Parameter out of range
  
  /* Set parameters */
  settings->DacSequence[0] = *(parPtr + 0);
	settings->DacSequence[1] = *(parPtr + 1);
  /* Print msg to inform user */
  comInterface->len = sprintf((char*)comInterface->buf,
  "Sequence set to: %u %u\n\r",
  settings->DacSequence[0],
		settings->DacSequence[1]);
  return true;
}
//Timer frequency for DAC
bool DacSetFreq (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
		//1. Parameter: DAC frequency
		if(*(parPtr + 0) < 0 || *(parPtr + 0) > DAC_CH_HIGHRANGE) return false; //Parameter out of range

		/* Set parameters */
		settings->DacSequence[0] = *(parPtr + 0);
		settings->DacSequence[1] = *(parPtr + 1);
		/* Print msg to inform user */
		comInterface->len = sprintf((char*)comInterface->buf,
		"Sequence set to: %u %u\n\r",
		settings->DacSequence[0],
		settings->DacSequence[1]);
		return true;
}

bool DacSetLutLength (int32_t *parPtr, daq_settings_t *settings, COM_t *comInterface)
{
	//1. Parameter: DAC channel
	//2. Parameter: LUT length
  if(*(parPtr + 0) < DAC_CH_LOWRANGE || *(parPtr + 0) > DAC_CH_HIGHRANGE
		|| *(parPtr + 1) < 1 || *(parPtr + 1) > DAC_LUT_LOCATION_HIGHRANGE+1 //Has to be above 0 and lower 1025
					 ) return false; //Parameter out of range
  
	  /* Set parameters */
	  settings->DAC[(uint16_t)(*parPtr)-1].LutLength = (uint16_t)*(parPtr + 1);
	  /* Print msg to inform user */
	  comInterface->len = sprintf((char*)comInterface->buf,
																															"$LUT length of DAC channel %u was set to %u\n\r",
																															(uint16_t)(*parPtr),
																															settings->DAC[(uint16_t)(*parPtr)-1].LutLength);
			return true;
}

/*********************************** end of parser.c ***********************************/