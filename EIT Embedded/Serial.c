/*----------------------------------------------------------------------------
 * Name:    Serial.c
 * Purpose: USART usage for STM32 to provide serial i/o
 * Version: V1.00
 * Modified from USART.c by Trang Le and Brendan Long on 1/29/10
 * Date of last modification: 2/21/10
 * Uses OS_Wait and OS_Signal for Semaphore-waiting
 * Lab 2 EE345M
 * TA: Glen Rhodes
 *	
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stm32f10x_lib.h>                        // STM32F10x Library Definitions
#include <stdio.h>
#include "STM32_Init.h"                           // STM32 Initialization
#include "OS.h"


/*----------------------------------------------------------------------------
  Notes:
  The length of the receive and transmit buffers must be a power of 2.
  Each buffer has a next_in and a next_out index.
  If next_in = next_out, the buffer is empty.
  (next_in - next_out) % buffer_size = the number of characters in the buffer.
 *----------------------------------------------------------------------------*/
#define TBUF_SIZE   256	     /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/
#define RBUF_SIZE   256      /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/

/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#if TBUF_SIZE < 2
#error TBUF_SIZE is too small.  It must be larger than 1.
#elif ((TBUF_SIZE & (TBUF_SIZE-1)) != 0)
#error TBUF_SIZE must be a power of 2.
#endif

#if RBUF_SIZE < 2
#error RBUF_SIZE is too small.  It must be larger than 1.
#elif ((RBUF_SIZE & (RBUF_SIZE-1)) != 0)
#error RBUF_SIZE must be a power of 2.
#endif

#define INPUT_LENGTH 16

/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
struct buf_st {
  unsigned int in;                                // Next In Index
  unsigned int out;                               // Next Out Index
  char buf [RBUF_SIZE];                           // Buffer
};

static struct buf_st rbuf = { 0, 0, };
#define SIO_RBUFLEN ((unsigned short)(rbuf.in - rbuf.out))

static struct buf_st tbuf = { 0, 0, };
#define SIO_TBUFLEN ((unsigned short)(tbuf.in - tbuf.out))

static unsigned int tx_restart = 1;               // NZ if TX restart is required

//Semaphores
static Sema4Type DataAvailable, RoomLeft;

/*----------------------------------------------------------------------------
  USART1_IRQHandler
  Handles USART1 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART1_IRQHandler (void) {
  volatile unsigned int IIR;
  struct buf_st *p;

    IIR = USART1->SR;
    if (IIR & USART_FLAG_RXNE) {                  // read interrupt
      USART1->SR &= ~USART_FLAG_RXNE;	          // clear interrupt

      p = &rbuf;

      if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) {
        p->buf [p->in & (RBUF_SIZE-1)] = (USART1->DR & 0x1FF);
        p->in++;
    	OS_Signal(&DataAvailable);

      }
    }

    if (IIR & USART_FLAG_TXE) {
      USART1->SR &= ~USART_FLAG_TXE;	          // clear interrupt

      p = &tbuf;

      if (p->in != p->out) {
        USART1->DR = (p->buf [p->out & (TBUF_SIZE-1)] & 0x1FF);
        p->out++;
        tx_restart = 0;
       	OS_Signal(&RoomLeft);

      }
      else {
        tx_restart = 1;
		USART1->CR1 &= ~USART_FLAG_TXE;		      // disable TX interrupt if nothing to send

      }
    }
}

/*------------------------------------------------------------------------------
  Serial_Init
  inputs, return: none
  initialize the buffers
 *------------------------------------------------------------------------------*/
void Serial_Open (void) {

    tbuf.in = 0;                                    // Clear com buffer indexes
    tbuf.out = 0;
    tx_restart = 1;

    rbuf.in = 0;
    rbuf.out = 0;
    OS_InitSemaphore( &DataAvailable, 0 );
	OS_InitSemaphore( &RoomLeft, TBUF_SIZE );
}

/*------------------------------------------------------------------------------
  Serial_SendChar
  input: c : character to be sent on serial line
  return: 0 if successful (unsuccessful return deprecated, waits)
  transmit a character
 *------------------------------------------------------------------------------*/
short Serial_SendChar (char c) {
  struct buf_st *p = &tbuf; 
                                                  // If the buffer is full, wait for it to empty
  //while (SIO_TBUFLEN >= TBUF_SIZE){;}
  OS_Wait(&RoomLeft);  
                                                  
  p->buf [p->in & (TBUF_SIZE - 1)] = c;           // Add data to the transmit buffer.
  p->in++;

  if (tx_restart) {                               // If transmit interrupt is disabled, enable it
    tx_restart = 0;
	USART1->CR1 |= USART_FLAG_TXE;		          // enable TX interrupt
  }
  return (0);
}



/*------------------------------------------------------------------------------
  Serial_GetChar
  receive a character
  * input: none
  return: the character pulled off the serial port's buffer.
 *------------------------------------------------------------------------------*/
char Serial_GetChar (void) {
  struct buf_st *p = &rbuf;
  char charRead;
 // while (SIO_RBUFLEN == 0)	 {;}
  OS_Wait(&DataAvailable); 
  charRead = p->buf [(p->out++) & (RBUF_SIZE - 1)];
  return charRead;
  
}

/*****************************************************************************
*  Serial_GetLine
*  Obtain a full line of text terminated by a new line character
*  input: *input: pointer to an array of chars to be filled with input from port
*			max: max length of input buffer
*	return: none (input stored in *input buffer
******************************************************************************/
void Serial_GetLine (char *input, unsigned short max) {
	unsigned long length = 0;
	char character;

	character = getchar();
	while (character != 0x0D) {     // check for new line character
		if(character == 0x08) //check for backspace
		{
			if(length)
			{
				input--;
				length--;
				printf("%c",0x08);
			}
		}
		else if(length < max)
		{
			*input = character;
			input++;
			length++;
			printf("%c", character);
		}
		character = getchar();
	}    
	*input = 0;
}
