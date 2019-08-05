/************************************************************************************//**
* \file     comInterface.c
* \brief    This module implements the USB interface.
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "comInterface.h"
#include <asf.h>
#include <stdio.h>


/***************************************************************************************
* Function prototypes
****************************************************************************************/
uint8_t init(void *ptr);
uint8_t available(void);
uint8_t read(void);
uint8_t write(const uint8_t *payload);
uint8_t printbuf(uint8_t *bufPtr, uint16_t len);


/****************************************************************************************
* Local data declarations
****************************************************************************************/
COM_t comInterface =
{
  init,
  available,
  read,
  write,
  printbuf,
};


/****************************************************************************************
*                    C O M U N I C A T I O N   U T I L I T I E S
****************************************************************************************/
/************************************************************************************//**
** \brief     Obtains a pointer to the USB interface structure.
** \return    Pointer to USB interface structure.
**
****************************************************************************************/
COM_t *getComInterface()
{
  return &comInterface;
} /*** end of getComInterface ***/


/************************************************************************************//**
** \brief     Initializes the USB interface.
** \param     settings Pointer to the USB interface settings.
**
****************************************************************************************/
uint8_t init(void *ptr) //initialize communication
{
  /* Set result to false. */
  bool result = 0;
  /* USB stack, ASF */
  udc_start();
  result = 1;
  /* return result */
  return result;
} /*** end of init ***/


/************************************************************************************//**
** \brief     Return length of data in receive buffer.
** \return    Length of data in receive buffer.
**
****************************************************************************************/
uint8_t available()
{
  return udi_cdc_get_nb_received_data();
} /*** end of available ***/


/************************************************************************************//**
** \brief     Return char from receive buffer.
** \return    Return next uint8_t from buffer.
**
****************************************************************************************/
uint8_t read()
{
  return udi_cdc_getc();
} /*** end of read ***/


/************************************************************************************//**
** \brief     Write single char to interface.
** \return    Return result.
**
****************************************************************************************/
uint8_t write(const uint8_t *payload)
{
  return udi_cdc_write_buf(&payload, 1);
} /*** end of write ***/


/************************************************************************************//**
** \brief     Print buffer to interface
** \return    Return result.
**
****************************************************************************************/
uint8_t printbuf(uint8_t *bufPtr, uint16_t len)
{
  return udi_cdc_multi_write_buf(0, bufPtr, len);
} /*** end of printbuf ***/


/****************************************************************************************
*                            C A L L B A C K   F U N C T I O N S
****************************************************************************************/
/************************************************************************************//**
** \brief     This is a callback function and it is called when we get new data.
**
****************************************************************************************/
void udi_cdc_callback_rx_notify(uint8_t port)
{
  if(comInterface.rxCallback != NULL)
  {
    comInterface.rxCallback(port);
  }
}


/************************************************************************************//**
** \brief     This is a callback function and it is called when transmit buffer is empty.
**
****************************************************************************************/
void udi_cdc_callback_tx_empty_notify(uint8_t port)
{
  if(comInterface.txEmptyCallback != NULL)
  {
    comInterface.txEmptyCallback(port);
  }
}


/******************************** end of comInterface.c ********************************/