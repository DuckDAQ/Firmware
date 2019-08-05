/************************************************************************************//**
* \file     comInterface.h
* \brief    This module implements the USB interface.
****************************************************************************************/
#ifndef COMINTERFACE_H_
#define COMINTERFACE_H_


/****************************************************************************************
* Include files
****************************************************************************************/
#include <stdio.h>


/****************************************************************************************
* Macro definitions
****************************************************************************************/
#ifndef comBufLen
  #define comBufLen 256
#endif


/****************************************************************************************
* Type definitions
****************************************************************************************/
typedef struct
{
  uint8_t (* init) (void*);
  uint8_t (* available) (void);
  uint8_t (* read) (void);
  uint8_t (* write) (const uint8_t*);
  uint8_t (* printBuf) (uint8_t*, uint16_t);
  void (* rxCallback) (uint8_t);
  void (* txEmptyCallback) (uint8_t);
  uint8_t buf[comBufLen];
  uint8_t len;
}COM_t;


/***************************************************************************************
* Function prototypes
****************************************************************************************/
COM_t *getComInterface(void);
/* Callback functions. */
void udi_cdc_callback_rx_notify(uint8_t port);
void udi_cdc_callback_tx_empty_notify(uint8_t port);


#endif /* COMINTERFACE_H_ */
/******************************** end of comInterface.h ********************************/