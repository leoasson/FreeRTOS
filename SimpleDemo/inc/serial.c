/****************************************************************************
 *   $Id:: uart.c 5751 2010-11-30 23:56:11Z usb00423                        $
 *   Project: NXP LPC17xx UART example
 *
 *   Description:
 *     This file contains UART code example which include UART initialization,
 *     UART interrupt handler, and APIs for UART access.
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
#include "LPC17xx.h"
#include <stdlib.h>
#include "serial.h"

volatile uint32_t UART0Status, UART1Status, UART3Status;
volatile uint8_t UART0TxEmpty = 1, UART1TxEmpty = 1, UART3TxEmpty=1;
volatile uint8_t UART0Buffer[UARTRXQUEUESIZE], UART1Buffer[UARTRXQUEUESIZE], UART3Buffer[UARTRXQUEUESIZE];
volatile uint32_t UART0RxQueueWritePos = 0, UART1RxQueueWritePos = 0, UART3RxQueueWritePos = 0;
volatile uint32_t UART0RxQueueReadPos = 0, UART1RxQueueReadPos = 0, UART3RxQueueReadPos = 0;

/*****************************************************************************
** Function name:		UART0_IRQHandler
**
** Descriptions:		UART0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void UART0_IRQHandler (void)
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;

  IIRValue = LPC_UART0->IIR;

  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
  {
	LSRValue = LPC_UART0->LSR;
	/* Receive Line Status */
	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
	{
	  /* There are errors or break interrupt */
	  /* Read LSR will clear the interrupt */
	  UART0Status = LSRValue;
	  Dummy = LPC_UART0->RBR;		/* Dummy read on RX to clear
							interrupt, then bail out */
	  return;
	}
	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */
	{
	  /* If no error on RLS, normal ready, save into the data buffer. */
	  /* Note: read RBR will clear the interrupt */
		  if ((UART0RxQueueWritePos+1) % UARTRXQUEUESIZE != UART0RxQueueReadPos)
		  {
			  UART0Buffer[UART0RxQueueWritePos] = LPC_UART0->RBR;
			  UART0RxQueueWritePos = (UART0RxQueueWritePos+1) % UARTRXQUEUESIZE;
		  }
	}
  }
  else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
	/* Receive Data Available */
	  if ((UART0RxQueueWritePos+1) % UARTRXQUEUESIZE != UART0RxQueueReadPos)
	  {
		  UART0Buffer[UART0RxQueueWritePos] = LPC_UART0->RBR;
		  UART0RxQueueWritePos = (UART0RxQueueWritePos+1) % UARTRXQUEUESIZE;
	  }
  }
  else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
  {
	/* Character Time-out indicator */
	UART0Status |= 0x100;		/* Bit 9 as the CTI error */
  }
  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
  {
	/* THRE interrupt */
	LSRValue = LPC_UART0->LSR;		/* Check status in the LSR to see if
									valid data in U0THR or not */
	if ( LSRValue & LSR_THRE )
	{
	  UART0TxEmpty = 1;
	}
	else
	{
	  UART0TxEmpty = 0;
	}
  }
}

/*****************************************************************************
** Function name:		UART1_IRQHandler
**
** Descriptions:		UART1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void UART1_IRQHandler (void)
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;

  IIRValue = LPC_UART1->IIR;

  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
  {
	LSRValue = LPC_UART1->LSR;
	/* Receive Line Status */
	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
	{
	  /* There are errors or break interrupt */
	  /* Read LSR will clear the interrupt */
	  UART1Status = LSRValue;
	  Dummy = LPC_UART1->RBR;		/* Dummy read on RX to clear
								interrupt, then bail out */
	  return;
	}
	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */
	{
	  /* If no error on RLS, normal ready, save into the data buffer. */
	  /* Note: read RBR will clear the interrupt */
	  if ((UART1RxQueueWritePos+1) % UARTRXQUEUESIZE != UART1RxQueueReadPos)
	  {
		  UART1Buffer[UART1RxQueueWritePos] = LPC_UART1->RBR;
		  UART1RxQueueWritePos =(UART1RxQueueWritePos+1) % UARTRXQUEUESIZE;
	  }
	}
  }
  else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
	/* Receive Data Available */
	  if ((UART1RxQueueWritePos+1) % UARTRXQUEUESIZE != UART1RxQueueReadPos)
	  {
		  UART1Buffer[UART1RxQueueWritePos] = LPC_UART1->RBR;
		  UART1RxQueueWritePos = (UART1RxQueueWritePos+1) % UARTRXQUEUESIZE;
	  }
  }
  else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
  {
	/* Character Time-out indicator */
	UART1Status |= 0x100;		/* Bit 9 as the CTI error */
  }
  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
  {
	/* THRE interrupt */
	LSRValue = LPC_UART1->LSR;		/* Check status in the LSR to see if
								valid data in U0THR or not */
	if ( LSRValue & LSR_THRE )
	{
	  UART1TxEmpty = 1;
	}
	else
	{
	  UART1TxEmpty = 0;
	}
  }
  //vTaskResume(xHandle);
  //xTaskResumeFromISR(xHandle);
}
/*****************************************************************************
** Function name:		UART3_IRQHandler
**
** Descriptions:		UART0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void UART3_IRQHandler (void)
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;

  IIRValue = LPC_UART3->IIR;

  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
  {
	LSRValue = LPC_UART3->LSR;
	/* Receive Line Status */
	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
	{
	  /* There are errors or break interrupt */
	  /* Read LSR will clear the interrupt */
	  UART3Status = LSRValue;
	  Dummy = LPC_UART3->RBR;		/* Dummy read on RX to clear
							interrupt, then bail out */
	  return;
	}
	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */
	{
		xTaskResumeFromISR(xHandle);
	  /* If no error on RLS, normal ready, save into the data buffer. */
	  /* Note: read RBR will clear the interrupt */
		 if ((UART3RxQueueWritePos+1) % UARTRXQUEUESIZE != UART3RxQueueReadPos)
		  {
			  UART3Buffer[UART3RxQueueWritePos] = LPC_UART3->RBR;
			  UART3RxQueueWritePos = (UART3RxQueueWritePos+1) % UARTRXQUEUESIZE;
		  }
	}
  }
  else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
	  xTaskResumeFromISR(xHandle);
	/* Receive Data Available */
	  if ((UART3RxQueueWritePos+1) % UARTRXQUEUESIZE != UART3RxQueueReadPos)
	  {
		  UART3Buffer[UART3RxQueueWritePos] = LPC_UART3->RBR;
		  UART3RxQueueWritePos = (UART3RxQueueWritePos+1) % UARTRXQUEUESIZE;
	  }
  }
  else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
  {
	/* Character Time-out indicator */
	UART3Status |= 0x100;		/* Bit 9 as the CTI error */
  }
  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
  {
	/* THRE interrupt */
	LSRValue = LPC_UART3->LSR;		/* Check status in the LSR to see if
									valid data in U0THR or not */
	if ( LSRValue & LSR_THRE )
	{
	  UART3TxEmpty = 1;
	}
	else
	{
	  UART3TxEmpty = 0;
	}
  }

  //vTaskResume(xHandle);
  //xTaskResumeFromISR( xHandle );
}


/*****************************************************************************
** Function name:		Serial_begin
**
** Descriptions:		Initialize UART port, setup pin select,
**						clock, parity, stop bits, FIFO, etc.
**
** parameters:			portNum(0,1 or 3) and UART baudrate
** Returned value:		true or false, return false only if the
**						interrupt handler can't be installed to the
**						VIC table
**
*****************************************************************************/
void Serial_begin( uint8_t PortNum, uint32_t baudrate )
{
  uint32_t Fdiv;
  uint32_t pclkdiv, pclk;

  if ( PortNum == 0 )
  {
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;  /* RxD0 is P0.3 and TxD0 is P0.2 */
	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 6~7 is for UART0 */
	pclkdiv = (LPC_SC->PCLKSEL0 >> 6) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemCoreClock/4;
		break;
	  case 0x01:
		pclk = SystemCoreClock;
		break;
	  case 0x02:
		pclk = SystemCoreClock/2;
		break;
	  case 0x03:
		pclk = SystemCoreClock/8;
		break;
	}

    LPC_UART0->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	Fdiv = ( pclk / 16 ) / baudrate ;	/*baud rate */
    LPC_UART0->DLM = Fdiv / 256;
    LPC_UART0->DLL = Fdiv % 256;
	LPC_UART0->LCR = 0x03;		/* DLAB = 0 */
    LPC_UART0->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

   	NVIC_EnableIRQ(UART0_IRQn);

    LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART0 interrupt */
  }
  else if ( PortNum == 1 )
  {
	LPC_PINCON->PINSEL4 &= ~0x0000000F;
	LPC_PINCON->PINSEL4 |= 0x0000000A;	/* Enable RxD1 P2.1, TxD1 P2.0 */

	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 8,9 are for UART1 */
	pclkdiv = (LPC_SC->PCLKSEL0 >> 8) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemCoreClock/4;
		break;
	  case 0x01:
		pclk = SystemCoreClock;
		break;
	  case 0x02:
		pclk = SystemCoreClock/2;
		break;
	  case 0x03:
		pclk = SystemCoreClock/8;
		break;
	}

    LPC_UART1->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	Fdiv = ( pclk / 16 ) / baudrate ;	/*baud rate */
    LPC_UART1->DLM = Fdiv / 256;
    LPC_UART1->DLL = Fdiv % 256;
	LPC_UART1->LCR = 0x03;		/* DLAB = 0 */
    LPC_UART1->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

   	NVIC_EnableIRQ(UART1_IRQn);

    LPC_UART1->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART1 interrupt */
  }
  else if ( PortNum == 3 )
  {
	  LPC_PINCON->PINSEL0 &= ~0x0000000F;
	  LPC_PINCON->PINSEL0 |=  0x0000000A;  /* RxD3 is P0.1 and TxD3 is P0.0 */
	  LPC_SC->PCONP |= 1<<4 | 1<<25; //Enable PCUART1
	  /* By default, the PCLKSELx value is zero, thus, the PCLK for
		all the peripherals is 1/4 of the SystemFrequency. */
	  /* Bit 6~7 is for UART3 */
	  pclkdiv = (LPC_SC->PCLKSEL1 >> 18) & 0x03;
	  switch ( pclkdiv )
	  {
	  case 0x00:
	  default:
		  pclk = SystemCoreClock/4;
		  break;
	  case 0x01:
		  pclk = SystemCoreClock;
		  break;
	  case 0x02:
		  pclk = SystemCoreClock/2;
		  break;
	  case 0x03:
		  pclk = SystemCoreClock/8;
		  break;
	  }
	  LPC_UART3->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	  Fdiv = ( pclk / 16 ) / baudrate ;	/*baud rate */
	  LPC_UART3->DLM = Fdiv / 256;
	  LPC_UART3->DLL = Fdiv % 256;
	  LPC_UART3->LCR = 0x03;		/* DLAB = 0 */
	  LPC_UART3->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

	  NVIC_EnableIRQ(UART3_IRQn);

	  LPC_UART3->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART3 interrupt */
  }
}

/*****************************************************************************
** Function name:		Serial_write
**
** Descriptions:		Send a byte of data to the UART
**
** parameters:			portNum(0,1 or 3), data byte
** Returned value:		None
**
*****************************************************************************/
void Serial_write( uint8_t portNum, uint8_t send)
{
  if ( portNum == 0 )
  {

	  /* THRE status, contain valid data */
	  while ( !(UART0TxEmpty & 0x01) );
	  LPC_UART0->THR = send;
	  UART0TxEmpty = 0;	/* not empty in the THR until it shifts out */


  }
  else if (portNum == 1)
  {

	  /* THRE status, contain valid data */
	  while ( !(UART1TxEmpty & 0x01) );
	  LPC_UART1->THR = send;
	  UART1TxEmpty = 0;	/* not empty in the THR until it shifts out */


  }
  else if ( portNum == 3 )
  {
	  /* THRE status, contain valid data */
	  while ( !(UART3TxEmpty & 0x01) );
	  LPC_UART3->THR = send;
	  UART3TxEmpty = 0;	/* not empty in the THR until it shifts out */

  }
  return;
}

/*****************************************************************************
** Function name:		Serial_printString
**
** Descriptions:		Send a zero terminated string to the UART
**
** parameters:			portNum(0,1 or 3), buffer pointer
** Returned value:		None
**
*****************************************************************************/
void Serial_printString( uint8_t portNum, char *BufferPtr)
{
	while( *BufferPtr )
	{
		Serial_write( portNum, *BufferPtr++);
	}

}

/*****************************************************************************
** Function name:		Serial_printNumber
**
** Descriptions:		Send a number to the UART as human-readable ASCII text
**
** parameters:			portNum(0,1 or 3), value, format
** Returned value:		None
**
*****************************************************************************/
void Serial_printNumber( uint8_t portNum, uint32_t Value , uint8_t base)
{
	char buf[10];
	itoa(Value, buf, base);
	//UARTSendLen(portNum, (uint8_t *)buf , strlen(buf) );
	Serial_printString(portNum, (char *)buf);
}

/*****************************************************************************
** Function name:		Serial_println
**
** Descriptions:		Send a CRLF sequence to the UART
**
** parameters:			portNum(0,1 or 3)
** Returned value:		None
**
*****************************************************************************/
void Serial_println ( uint32_t portNum )
{
	Serial_write(portNum, 13);
	Serial_write(portNum, 10);
}

/*****************************************************************************
** Function name:		Serial_printcr
**
** Descriptions:		Send a CR to the UART
**
** parameters:			portNum(0,1 or 3)
** Returned value:		None
**
*****************************************************************************/
void Serial_printcr ( uint32_t portNum )
{
	Serial_write(portNum, 13);
}

/*****************************************************************************
** Function name:		Serial_available
**
** Descriptions:		Returns the bytes inside the RX FIFO ringbuffer
**
** parameters:			portNum(0,1 or 3)
** Returned value:		number of bytes in the RX FIFO ringbuffer
**
*****************************************************************************/
uint8_t Serial_available (uint32_t portNum)
{
	  if ( portNum == 0 )
	  {
		  return (UART0RxQueueWritePos + UARTRXQUEUESIZE - UART0RxQueueReadPos) % UARTRXQUEUESIZE;
	  }
	  if ( portNum == 1 )
	  {
		  return (UART1RxQueueWritePos + UARTRXQUEUESIZE - UART1RxQueueReadPos) % UARTRXQUEUESIZE;
	  }
	  if ( portNum == 3 )
	  {
		  return (UART3RxQueueWritePos + UARTRXQUEUESIZE - UART3RxQueueReadPos) % UARTRXQUEUESIZE;
	  }
	  return 0;
}

/*****************************************************************************
** Function name:		Serial_read
**
** Descriptions:		Reads incoming serial data
**
** parameters:			portNum(0,1 or 3)
** Returned value:		the first byte of incoming serial data available (or -1 if no data is available)
**
*****************************************************************************/
int16_t Serial_read (uint32_t portNum)
{
	uint8_t rx;
	if ( portNum == 0 )
	  {
		  if (UART0RxQueueReadPos == UART0RxQueueWritePos)
		    return -1;

		  // Read from "head"
		  rx = UART0Buffer[UART0RxQueueReadPos]; // grab next byte
		  UART0RxQueueReadPos = (UART0RxQueueReadPos + 1) % UARTRXQUEUESIZE;
		  return rx;
	  }
	  if ( portNum == 1 )
	  {
		  if (UART1RxQueueReadPos == UART1RxQueueWritePos)
		    return -1;

		  // Read from "head"
		  rx = UART1Buffer[UART1RxQueueReadPos]; // grab next byte
		  UART1RxQueueReadPos = (UART1RxQueueReadPos + 1) % UARTRXQUEUESIZE;
		  return rx;
	  }
	  if ( portNum == 3 )
	  {
		  if (UART3RxQueueReadPos == UART3RxQueueWritePos)
		    return -1;

		  // Read from "head"
		  rx = UART3Buffer[UART3RxQueueReadPos]; // grab next byte
		  UART3RxQueueReadPos = (UART3RxQueueReadPos + 1) % UARTRXQUEUESIZE;
		  return rx;
	  }
	  return 0;
}

/*****************************************************************************
** Function name:		Serial_flush
**
** Descriptions:		Clears the RX FIFO ringbuffer
**
** parameters:			portNum(0,1 or 3)
** Returned value:		none
**
*****************************************************************************/
uint8_t Serial_flush (uint32_t portNum)
{
	  if ( portNum == 0 )
	  {
		  UART0RxQueueWritePos = 0;
		  UART0RxQueueReadPos = 0;

	  }
	  if ( portNum == 1 )
	  {
		  UART1RxQueueWritePos = 0;
		  UART1RxQueueReadPos = 0;
	  }
	  if ( portNum == 3 )
	  {
		  UART3RxQueueWritePos = 0;
		  UART3RxQueueReadPos = 0;
	  }
	  return 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
