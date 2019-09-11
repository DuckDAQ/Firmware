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
		master_settings.syncBytes[1] = 0b10000000; //Sync bytes for 16 bytes blockSize
		master_settings.syncBytes[0] = 0b10010000;
  master_settings.ADCgain[0] = 1;
  master_settings.ADCgain[1] = 1;
  master_settings.ADCgain[2] = 1;
  master_settings.ADCgain[3] = 1;
  master_settings.ADClowRes = false;
  master_settings.com = USB;
  
  for(uint16_t i = 0; i < DACC_BUFFER_SIZE*2; i++){ //sawtooth wave
				if(i%2 == 0) master_settings.Lut[i] = (4095 - i*2); //Channel 1 (First, odd buffer indexes)
				else master_settings.Lut[i] = (i*2)|(1<<12); //Channel 2 
  }
		master_settings.LutLength = 1024;
		master_settings.NumOfRepeats = 0;
		master_settings.DacPeriod = 2000; //us -> Sawtooth freq = 1/(5000 *10^-6s * 1024) 
		  
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