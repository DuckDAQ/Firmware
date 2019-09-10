#include <asf.h>
#include "comInterface.h"
#include "core.h"
#include "parser.h"


void usbRxcallback(uint8_t port);


COM_t * USB;
daq_settings_t master_settings;


int main (void)
{
  wdt_disable(WDT);
  sysclk_init(); // clock init, ASF
  board_init();  // ASF function, empty function??
  
  
  /* Initialize pointer to communicate interface */
  USB = getComInterface();
  /* Initialize pointers to callback function. */
  USB->rxCallback = usbRxcallback;
  USB->txEmptyCallback = coreTxEmptyCallBack();
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
  master_settings.ADCgain[0] = 1;
  master_settings.ADCgain[1] = 1;
  master_settings.ADCgain[2] = 1;
  master_settings.ADCgain[3] = 1;
  master_settings.ADClowRes = false;
  master_settings.com = USB;
  
  for(uint16_t i = 0; i < 1024; i++){ //sawtooth wave
	  master_settings.DAC[0].Lut[i] = i*4;
	  master_settings.DAC[1].Lut[i] = 4095 - i*4;
  }
		master_settings.DAC[0].LutLength = 1024;
		master_settings.DAC[1].LutLength = 1024;
		master_settings.DAC[0].NumOfRepeats = 0;
		master_settings.DAC[1].NumOfRepeats = 0;
  master_settings.DacSequence[0] = 1;
  master_settings.DacSequence[1] = 2;
  
  coreConfigure(&master_settings);
  coreStart();
  
  while(1)
  {
    // Do something...
  }
  
}


void usbRxcallback(uint8_t port)
{
  CMD_t incomingCMD;
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
        USB->len = sprintf((char*)USB->buf, "ERROR setting command\n\r");
      }
    } 
    else
    {
      USB->len = sprintf((char*)USB->buf, "Command syntax ERROR\n\r");
    }
    USB->printBuf(USB->buf, USB->len);
  }
}