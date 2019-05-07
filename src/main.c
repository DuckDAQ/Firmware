#include <asf.h>
#include "comInterface.h"
#include "core.h"
#include "parser.h"


int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	wdt_disable(WDT);
	sysclk_init(); // clock init, ASF
	board_init();  // ASF function, empty function??
	core_init(); //core.c init
  
  /* Initialize pointer to communicate interface */
  COM_t *USB = getComInterface();
  /* Initialize communicate interface */
  USB->init(NULL);
  
  /* Structure with all the settings */
  daq_settings_t master_settings;
  
  /* Fill with some presets. */
  master_settings.acquisitionNbr = 4;
  master_settings.acqusitionTime = 10000;
  master_settings.averaging = 6;
  master_settings.channels = (DAQ_CHANNEL_1);
  master_settings.com = USB;

  while(1)
  {
    CMD_t incomingCMD;
    if(USB->available())
    {
      if (parseCommand(USB->read(), &incomingCMD, USB))
      {
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
}