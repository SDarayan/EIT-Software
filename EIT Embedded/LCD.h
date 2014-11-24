/*----------------------------------------------------------------------------
 * Name:    LCD.h
 * Purpose: Functions for 2 line 16 character Text LCD (4-bit interface)
 *                connected on MCBSTM32 evaluation board 
 * Version: V1.10
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 * Modified by Trang Le, Brendan Long
 * Initially Modified on 01/27/2010
 * Lab 1
 * TA: Glen Rhodes
 * Date of last revision: 02/1/2010
 * Hardware Configuration: 
  PINS: 
   - DB4 = PC3
   - DB5 = PC2
   - DB6 = PC1
   - DB7 = PC0
   - E   = PC10
   - RW  = PC11
   - RS  = PC12  
 *---------------------------------------------------------------------------*/
 #ifndef LCD
 #define LCD 1
 #include <stm32f10x_lib.h>              /* STM32F10x Library Definitions      */
 #include "OS.h"
 #include <string.h>
 #include <stdio.h>
/*******************************************************************************
* Initialize the LCD controller                                                *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_Init (void);

/*******************************************************************************
* Set cursor position on LCD display                                           *
*   Parameter:    screen: 1 top, 2 bottom                                      *
*                 column: column position                                      *
*                 line:   line position                                        *
*   Return:                                                                    *
*******************************************************************************/

void LCD_SetCursor (long column, long screen);

/*******************************************************************************
* Clear the LCD display                                                        *
*   Parameter: screen: 1 for top line, 2 for bottom line.                      *
*   Return:                                                                    *
*******************************************************************************/

void LCD_Clear (long screen);

/*******************************************************************************
* Print sting to LCD display                                                   *
*   Parameter:    string: pointer to output string                             *
*	1 for top line, 2 for bottom line. 										   *
*   Return:                                                                    *
*******************************************************************************/
void LCD_Print (char *string, long screen);

/*******************************************************************************
* Print Character to current cursor position                                   *
* Note: Does not increment cursor.                                             *
*   Parameter:    c:      character to be printed                              *
*                 screen: 0 top, 1 bottom                                      *						   
*   Return:                                                                    *
*******************************************************************************/
void LCD_PutChar (char c, long screen);

/*******************************************************************************
* Print unsigned long to current cursor position                               *
* Note: Does not increment cursor.                                             *
* maximum length of string representing number is 16.						   *
*   Parameter:    output: number to be printed                                 *
*                 screen: 0 top, 1 bottom                                      *						   
*   Return:                                                                    *
*******************************************************************************/
void LCD_PrintUInt(unsigned long output, long screen);

/*******************************************************************************
* Print a message including a string and a value							   *
* 	Parameter:		display: 0 top screen, 1 bottom screen					   *
*					pt	: string to be printed								   *	
*					data: number to be printed								   *
*	Return: none															   *
*******************************************************************************/
void LCD_Message(int display, char *pt, unsigned long data);
#endif
