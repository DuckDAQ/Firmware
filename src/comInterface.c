/************************************************************************************//**
* \file     comInterface.c
* \brief    This module implements the USB interface.
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include <asf.h>                     /* Atmel Software Framework include file          */
#include <stdio.h>                   /* Standard I/O functions.                        */
#include "comInterface.h"            /* USB interface driver                           */


/***************************************************************************************
* Function prototypes
****************************************************************************************/
uint8_t init(void *ptr);             /* Initialize comunication interface              */
uint8_t available(void);             /* Returns number of bytes in receive buffer      */
uint8_t read(void);                  /* Reads single byte from receive buffer          */
uint8_t write(uint8_t *payload);     /* Writes single byte to the interface            */
uint8_t printbuf(uint8_t *bufPtr, uint16_t len);  /* Writes buffer to the interface    */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Comunication interface structure */
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
uint8_t write(uint8_t *payload)
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
*                        C A L L B A C K   F U N C T I O N S
****************************************************************************************/
/************************************************************************************//**
** \brief     This is a callback function and it is called when we get new data.
**
****************************************************************************************/
void udi_cdc_callback_rx_notify(uint8_t port)
{
  /* Only continue if callback function was linked. */
  if(comInterface.rxCallback != NULL)
  {
    comInterface.rxCallback();
  }
} /*** end of udi_cdc_callback_rx_notify ***/


/************************************************************************************//**
** \brief     This is a callback function and it is called when transmit buffer is empty.
**
****************************************************************************************/
void udi_cdc_callback_tx_empty_notify(uint8_t port)
{
  /* Only continue if callback function was linked. */
  if(comInterface.txEmptyCallback != NULL)
  {
    comInterface.txEmptyCallback();
  }
} /*** end of udi_cdc_callback_tx_empty_notify ***/


/******************************** end of comInterface.c ********************************/