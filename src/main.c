/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "comInterface.h"
#include "core.h"
#include "parser.h"


//daq_measured_data_t calc_data;
//uint16_t debug_out [40];


/*
void delay (void)
{
	volatile uint32_t n;
	for(n = 0; n < 10000; n++) {}
}

void print_data (void)
{
	uint8_t str [40];
	uint32_t n;
	uint32_t chars = 0;
	for(n = 0; n < 4; n++)
	{
		if(master_settings.channels & (1 << n))
		{
			//!Warning		format '%u' expects argument of type 'unsigned int', but argument 3 has type 'long unsigned int' [-Wformat=]
			chars += sprintf(str, "CH%u: %u\n\r", n + 1, calc_data.results[n]);
		}
	}
	udi_cdc_write_buf(str, chars);
}

void debug_copy_data (void)
{
	static uint32_t n = 0;
	debug_out[n] = calc_data.results[0];
	debug_out[n + 1] = calc_data.results[1];
	n += 2;
}

void calculate_data (void)
{
	//!Warning		assignment from incompatible pointer type [-Wincompatible-pointer-types]
	uint32_t* raw_data_ptr;
	uint32_t n=0;
	//uint32_t m = 0;

	//dobi pointer na buffer, kjer so shranjeni ADC values
	raw_data_ptr = core_get_raw_data_pntr();
	for(n = 0; n < 4; n++)
	{
		calc_data.results[n] = *(raw_data_ptr + n) / master_settings.averaging;
	}
	calc_data.new_data = 1;
	core_clear_avg_acuum();
}
*/


int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	wdt_disable(WDT);
	sysclk_init(); // clock init, ASF
	board_init();  // ASF function, empty function??
	core_init(); //core.c init
  
  COM_t *USB = getComInterface();
  USB->init(NULL);

  daq_settings_t master_settings;
  
  master_settings.acquisitionNbr = 4;
  master_settings.acqusitionTime = 10000;
  master_settings.averaging = 6;
  master_settings.channels = (DAQ_CHANNEL_1);

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
  
  /*
	while(1)
	{

		if(udi_cdc_get_nb_received_data()) //usb recieved data, return the number of data available
		{
			//?vrjetn bi mogu poslt udi_cdc_getc() parserju
			if(udi_cdc_getc() == 's') //Waits and gets a value on CDC line; return value read on CDC line [int]
			{
				core_configure(&master_settings); //nastavitev cora z daq_settingsi, ki mu jih passamo
				core_start(); //start core
				while(core_status_get() == CORE_RUNNING)
				{
					if(core_new_data_ready()) //return new_data [bool]
					{
						//calculate_data(); //povprecenje
						//debug_copy_data();
						core_new_data_claer(); //new_data=0
						//print_data(); //Posl podatke o ADC prek USB na komp
					}
				}
			}
		}
	}
  */
  
}
