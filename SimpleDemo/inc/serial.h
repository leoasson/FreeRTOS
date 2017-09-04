/****************************************************************************
 *   $Id:: uart.h 5751 2010-11-30 23:56:11Z usb00423                        $
 *   Project: NXP LPC17xx UART example
 *
 *   Description:
 *     This file contains UART code header definition.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

#define HEX 16
#define DEC 10
#define BIN 2

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;


#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

#define UARTRXQUEUESIZE		0x10

void Serial_begin( uint8_t PortNum, uint32_t baudrate );
void Serial_write( uint8_t portNum, uint8_t send);
void Serial_printString( uint8_t portNum, char *BufferPtr);
void Serial_printNumber( uint8_t portNum, uint32_t Value , uint8_t base);
void Serial_println ( uint32_t portNum );
void Serial_printcr ( uint32_t portNum );
uint8_t Serial_available (uint32_t portNum);
int16_t Serial_read (uint32_t portNum);
uint8_t Serial_flush (uint32_t portNum);

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
xTaskHandle xHandle;

#endif /* end __SERIAL_H */
