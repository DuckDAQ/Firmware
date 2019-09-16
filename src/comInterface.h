/************************************************************************************//**
* \file     comInterface.h
* \brief    This module implements the USB interface.
****************************************************************************************/
#ifndef COMINTERFACE_H_
#define COMINTERFACE_H_


/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdio.h>                   /* Standard I/O functions.                        */


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#ifndef comBufLen                   /* If size not yet defined, define default.        */
  #define comBufLen 256
#endif


/****************************************************************************************
* Type definitions
****************************************************************************************/
/** \brief Comunication interface structure */
typedef struct
{
  uint8_t (* init) (void*);                 /* Initialize comunication interface       */
  uint8_t (* available) (void);             /*Returns number of bytes in receive buffer*/
  uint8_t (* read) (void);                  /* Reads single byte from receive buffer   */
  uint8_t (* write) (uint8_t*);             /* Writes single byte to the interface     */
  uint8_t (* printBuf) (uint8_t*, uint16_t);/* Writes buffer to the interface          */
  void    (* rxCallback) (void);            /* Pointer to the callback funcion handler */
  void    (* txEmptyCallback) (void);       /* Pointer to the callback funcion handler */
  uint8_t buf[comBufLen];                   /* Communication transmit buffer           */
  uint8_t len;                              /* Communication transmit buffer lenght    */
}COM_t;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
COM_t *getComInterface(void);               /* Gets struct of communiaction interface  */
/* Callback functions. */
void udi_cdc_callback_rx_notify(uint8_t port);       /* UDI specific callback function */
void udi_cdc_callback_tx_empty_notify(uint8_t port); /* UDI specific callback function */


#endif /* COMINTERFACE_H_ */
/******************************** end of comInterface.h ********************************/