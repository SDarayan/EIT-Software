/*----------------------------------------------------------------------------
 * Name:    Serial.h
 * Purpose: prototypes for USART usage for STM32 to provide serial i/o
 * Version: V1.00
 * Modified from USART.c by Trang Le and Brendan Long on 1/29/10
 * Date of last modification: 2/8/10
 * Lab 1 EE345M
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
 #ifndef __SERIAL_H
 #define __SERIAL_H 1

/*------------------------------------------------------------------------------
  Serial_Init
  inputs, return: none
  initialize the buffers
 *------------------------------------------------------------------------------*/
void Serial_Open (void) ;

/*------------------------------------------------------------------------------
  Serial_SendChar
  input: c : character to be sent on serial line
  return: 0 if successful (unsuccessful return deprecated, waits)
  transmit a character
 *------------------------------------------------------------------------------*/
short Serial_SendChar (char c) ;

/*------------------------------------------------------------------------------
  Serial_GetChar
  receive a character
  * input: none
  return: the character pulled off the serial port's buffer.
 *------------------------------------------------------------------------------*/
char Serial_GetChar (void) ;

/*****************************************************************************
*  Serial_GetLine
*  Obtain a full line of text terminated by a new line character
*  input: *input: pointer to an array of chars to be filled with input from port
*			max: max length of input buffer
*	return: none (input stored in *input buffer
******************************************************************************/
void Serial_GetLine (char *input, unsigned short max) ;

 #endif
