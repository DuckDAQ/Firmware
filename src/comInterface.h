/*
* communication.h
*
* Created: 9.4.2019 21:17:07
*  Author: Hrastovc
*/


#ifndef COMINTERFACE_H_
#define COMINTERFACE_H_

#include <stdio.h>

#ifndef comBufLen
#define comBufLen 100
#endif

typedef struct
{
	uint8_t (*init)(void*);
	uint8_t (*available)(void);
	uint8_t (*read)(void);
	uint8_t (*write)(uint8_t*);
	uint8_t (*printBuf)(uint8_t*, uint8_t);
	uint8_t buf[comBufLen];
	uint8_t len;
}COM_t;

COM_t *getComInterface(void);

#endif /* COMINTERFACE_H_ */