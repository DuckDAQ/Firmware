/*
 * communication.c
 *
 * Created: 9.4.2019 21:16:30
 *  Author: Hrastovc
 */ 

#include "comInterface.h"
#include <asf.h>
#include <stdio.h>


//function prototypes
uint8_t init(void *ptr);
uint8_t available(void);
uint8_t read(void);
uint8_t write(uint8_t *payload);
uint8_t printbuf(uint8_t *bufPtr, uint8_t len);


//pointers to functions
COM_t comInterface =
{
  init,
  available,
  read,
  write,
  printbuf,
};


//return pointers to functions
COM_t *getComInterface()
{
  return &comInterface;
}


//functions
uint8_t init(void *ptr) //initialize communication
{
  bool result = 0;	//set result to false
  
  udc_start();  //USB stack, ASF
  result = 1;
  
  return result;	//return result
}

uint8_t available() //return length of data in receive buffer
{
  return udi_cdc_get_nb_received_data();  //return length of data in receive buffer
}

uint8_t read()  //return char from receive buffer
{
  return udi_cdc_getc();	//return char
}

uint8_t write(uint8_t *payload) //write single char
{
  return udi_cdc_write_buf(&payload, 1);	//return result
}

uint8_t printbuf(uint8_t *bufPtr, uint8_t len)  //print buffer
{
  return udi_cdc_write_buf(bufPtr, len);	//return result
}
