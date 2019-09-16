/************************************************************************************//**
* \file     core.c
* \brief    Main program entry point
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <asf.h>                     /* Atmel Software Framework include file          */
#include "comInterface.h"            /* USB interface driver                           */
#include "core.h"                    /* Core functionality for acquisition             */
#include "parser.h"                  /* Protocol parser with command handlers          */


/***************************************************************************************
* Function prototypes
****************************************************************************************/
void usbRxcallback(void);            /* Current interface receive callback handler     */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Comunication interface structure */
COM_t * USB;
/** \brief Structure with all the core specific settings */
daq_settings_t master_settings;


/************************************************************************************//**
** \brief     This is main program entry point
** \return    TRUE
**
****************************************************************************************/
int main(void)
{
  /* WatchDog timer is currently not implemented. */
  wdt_disable(WDT);
  /* Initialize system clock controller. */
  sysclk_init();
  /* Initialize hardware specific settings. */
  board_init();
  
  /* Initialize pointer to communicate interface, in this case USB. */
  USB = getComInterface();
  /* Initialize pointers to callback function. */
  USB->rxCallback = usbRxcallback;
  USB->txEmptyCallback = coreGetTxEmptyCallBackPtr();
  /* Initialize communicate interface */
  USB->init(NULL);
  
  /* Fill with some presets. */
  master_settings.acquisitionNbr = 0;
  master_settings.acqusitionTime = 1000000;
  master_settings.averaging = 0;
  master_settings.sequence[0] = 1;
  master_settings.sequence[1] = 2;
  master_settings.sequence[2] = 3;
  master_settings.sequence[3] = 4;
  master_settings.mode = ASCII_MODE;
  master_settings.DACval[0] = 2047;
  master_settings.DACval[1] = 2047;
  master_settings.blockSize = 16;
  master_settings.syncBytes[0] = ( (uint16_t)master_settings.blockSize       & 0b1111111)
                                 | 0b10000000;
  master_settings.syncBytes[1] = (((uint16_t)master_settings.blockSize >> 7) & 0b1111111)
                                 | 0b10000000;
  master_settings.ADCgain[0] = 1;
  master_settings.ADCgain[1] = 1;
  master_settings.ADCgain[2] = 1;
  master_settings.ADCgain[3] = 1;
  master_settings.ADClowRes = false;
  
  /* Fill DAC LUT table with sawtooth signal. */
  for(uint16_t i = 0; i < DACC_BUFFER_SIZE*2; i++)
  {
    /* When odd index value is for channel 1, otherwise channel 2. */
    if(i%2 == 0) master_settings.Lut[i] = (4095 - i*2);
    else         master_settings.Lut[i] = (i*2) | (1 << 12);
  }
  master_settings.LutLength = 1024;
  master_settings.NumOfRepeats = 0;
  master_settings.DacPeriod = 2000; //us -> Sawtooth freq = 1/(5000 *10^-6s * 1024) 
  
  /* Set pointer to linked interface driver. */
  master_settings.com = USB;
  
  /* Make settings acesable from core module. */
  coreConfigure(&master_settings);
  
  /* Start core module (timers, ADC, DAC, PDC). */
  coreStart();
  
  while(1)
  {
    // Do something...
  }
  
  return TRUE;
  
} /*** end of main ***/


/************************************************************************************//**
** \brief     Callback function for all the incoming data.
**
****************************************************************************************/
void usbRxcallback(void)
{
  /* Create command packet */
  CMD_t incomingCMD;
  /* Is anything in RX buffer to read? */
  if(USB->available())
  {
    /*
     * Parse incoming command from USB, save result in incomingCMD struct.
     * If command is supported, function returns int > 0.
     */
    if (parseCommand(USB->read(), &incomingCMD, USB))
    {
      /* Executes function that was set in incomingCMD */
      if(!incomingCMD.funcPtr(incomingCMD.par, &master_settings, USB))
      {
        /* Unable to set command with parameters, inform user. */
        USB->len = sprintf((char*)USB->buf, "ERROR setting command\n\r");
      }
    }
    /* Command not parsed properly */
    else
    {
      /* Command syntax in probably wrong, inform user. */
      USB->len = sprintf((char*)USB->buf, "Command syntax ERROR\n\r");
    }
    /* Print message */
    USB->printBuf(USB->buf, USB->len);
  }
} /*** end of usbRxcallback ***/


/************************************ end of main.c ************************************/