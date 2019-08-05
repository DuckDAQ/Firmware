/**
 * \file
 *
 * \brief User board configuration template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

//PCB Clock Sources & Settings
#define BOARD_FREQ_SLCK_XTAL     (32768UL)      //Slow clock XTAL frequency [Hz]
#define BOARD_FREQ_SLCK_BYPASS   (32768UL)      //External slow clock frequency [Hz]
#define BOARD_FREQ_MAINCK_XTAL   (16000000UL)   //Main clock XTAL frequency [Hz]
#define BOARD_FREQ_MAINCK_BYPASS (16000000UL)   //External main clock frequency [Hz]
#define BOARD_OSC_STARTUP_US     (15625UL)      //Main clock XTAL start-up time [us]

#endif // CONF_BOARD_H
