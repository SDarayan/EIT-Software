/*----------------------------------------------------------------------------
 * Name:    LCD.c
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
 * Modified on 01/27/2010
 * Lab 2
 * TA: Glen Rhodes
 * Date of last revision: 02/22/10
 * Uses OS_Wait and OS_Signal to implement Semaphore waiting for 
 *  mutual exclusion
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

#include "LCD.h"

/*********************** Hardware specific configuration **********************/

/*------------------------- Speed dependant settings -------------------------*/

/* If processor works on high frequency delay has to be increased, it can be 
   increased by factor 2^N by this constant                                   */
#define DELAY_2N     0

/*------------------------- Text LCD size definitions ------------------------*/

#define LineLen     16                  /* Width (in characters)              */
#define NumLines     2                  /* Hight (in lines)                   */

/*-------------------- LCD interface hardware definitions --------------------*/

/* PINS: 
   - DB4 = PC3
   - DB5 = PC2
   - DB6 = PC1
   - DB7 = PC0
   - E   = PC10
   - RW  = PC11
   - RS  = PC12                                                               */

#define PIN_E                 (   1 << 10)
#define PIN_RW                (   1 << 11)
#define PIN_RS                (   1 << 12)
#define PINS_CTRL             (0x07 << 10)
#define PINS_DATA             (0x0F <<  0)
#define PINS_ALL              (PINS_CTRL | PINS_DATA)

const unsigned int SWAP_DATA[16] = { 0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE, 
                                     0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF};

/* Enable Clock for peripheral driving LCD pins                               */
#define LCD_CLOCK_EN         (RCC->APB2ENR |= (1 << 4)); // enable clock for GPIOC

/* pin E  setting to 0 or 1                                                   */
#define LCD_E(x)              GPIOC->ODR = (GPIOC->ODR & ~PIN_E)  | (x ? PIN_E : 0);

/* pin RW setting to 0 or 1                                                   */
#define LCD_RW(x)             GPIOC->ODR = (GPIOC->ODR & ~PIN_RW) | (x ? PIN_RW : 0);

/* pin RS setting to 0 or 1                                                   */
#define LCD_RS(x)             GPIOC->ODR = (GPIOC->ODR & ~PIN_RS) | (x ? PIN_RS : 0);

/* Reading DATA pins                                                          */
#define LCD_DATA_IN           SWAP_DATA[(((GPIOC->IDR & PINS_DATA) >> 0) & 0x0F)]

/* Writing value to DATA pins                                                 */
#define LCD_DATA_OUT(x)       GPIOC->ODR = (GPIOC->ODR & ~PINS_DATA) | ((SWAP_DATA[x]) << 0);

/* Setting all pins to output mode                                            */
#define LCD_ALL_DIR_OUT       GPIOC->CRL = (GPIOC->CRL & 0xFFFF0000) | 0x00003333; \
                              GPIOC->CRH = (GPIOC->CRH & 0xFFF000FF) | 0x00033300;
 
/* Setting DATA pins to input mode                                            */
#define LCD_DATA_DIR_IN       GPIOC->CRL = (GPIOC->CRL & 0xFFFF0000) | 0x00004444;

/* Setting DATA pins to output mode                                           */
#define LCD_DATA_DIR_OUT      GPIOC->CRL = (GPIOC->CRL & 0xFFFF0000) | 0x00003333;

/******************************************************************************/

Sema4Type LCDFree;


/* 8 user defined characters to be loaded into CGRAM (used for bargraph)      */
const char UserFont[8][8] = {
  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
  { 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10 },
  { 0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18 },
  { 0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C },
  { 0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E },
  { 0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F },
  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
  { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 }
};


static long cursor0, cursor1 = 0;

/*******************************************************************************
* Delay in while loop cycles                                                   *
*   Parameter:    cnt:    number of while cycles to delay                      *
*   Return:                                                                    *
*******************************************************************************/

static void delay (unsigned cnt)
{
  cnt <<= DELAY_2N;

  while (cnt--);
}


/*******************************************************************************
* Read status of LCD controller                                                *
*   Parameter:    none                                                         *
*   Return:       Status byte contains busy flag and address pointer           *
*******************************************************************************/

static unsigned char lcdReadStatus (void)
{
  unsigned char status;

  LCD_DATA_DIR_IN
  LCD_RS(0)
  LCD_RW(1)
  delay(10);
  LCD_E(1)
  delay(10);
  status  = LCD_DATA_IN << 4;
  LCD_E(0)
  delay(10);
  LCD_E(1)
  delay(10);
  status |= LCD_DATA_IN;
  LCD_E(0)
  LCD_DATA_DIR_OUT
  return (status);
}


/*******************************************************************************
* Wait until LCD controller busy flag is 0                                     *
*   Parameter:                                                                 *
*   Return:       Status byte of LCD controller (busy + address)               *
*******************************************************************************/

static unsigned char waitWhileBusy (void)
{
  unsigned char status;

  do  {
    status = lcdReadStatus();
  }  while (status & 0x80);             /* Wait for busy flag                 */

  return (status);
}


/*******************************************************************************
* Write 4-bits to LCD controller                                               *
*   Parameter:    c:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

static void lcdWrite4Bit (unsigned char c)
{
  LCD_RW(0)
  LCD_E(1)
  LCD_DATA_OUT(c&0x0F)
  delay(10);
  LCD_E(0)
  delay(10);
}


/*******************************************************************************
* Write command to LCD controller                                              *
*   Parameter:    c:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

static void lcdWriteCmd (unsigned char c)
{
  waitWhileBusy();

  LCD_RS(0)
  lcdWrite4Bit (c>>4);
  lcdWrite4Bit (c);
}


/*******************************************************************************
* Write data to LCD controller                                                 *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static void lcdWriteData (unsigned char c)
{
  waitWhileBusy();

  LCD_RS(1)
  lcdWrite4Bit (c>>4);
  lcdWrite4Bit (c);
}


/*******************************************************************************
* Print Character to current cursor position                                   *
*   Parameter:    c:      character to be printed                              *
*                 screen: 0 top, 1 bottom                                      *
*   Return:                                                                    *
*******************************************************************************/

void lcdPutchar (char c)
{ 
  lcdWriteData (c);
}


/*******************************************************************************
* Initialize the LCD controller                                                *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_Init (void)
{ 
  
  int i;
  char const *p;

  OS_InitSemaphore(&LCDFree, 1);
  OS_bWait(&LCDFree);
  LCD_CLOCK_EN                          /* Enable clock for peripheral        */

  /* Set all pins for LCD as outputs                                          */
  LCD_ALL_DIR_OUT

  delay (15000);
  LCD_RS(0)
  lcdWrite4Bit (0x3);                 /* Select 4-bit interface             */
  delay (4100);
  lcdWrite4Bit (0x3);
  delay (100);
  lcdWrite4Bit (0x3);
  lcdWrite4Bit (0x2);

  lcdWriteCmd (0x28);                 /* 2 lines, 5x8 character matrix      */
  lcdWriteCmd (0x0C);                 /* Display ctrl:Disp=ON,Curs/Blnk=OFF */
  lcdWriteCmd (0x06);                 /* Entry mode: Move right, no shift   */

  /* Load user-specific characters into CGRAM                                 */
  lcdWriteCmd(0x40);                  /* Set CGRAM address counter to 0     */
  p = &UserFont[0][0];
  for (i = 0; i < sizeof(UserFont); i++, p++)
    lcdPutchar (*p);

  lcdWriteCmd(0x80);                  /* Set DDRAM address counter to 0     */
  OS_bSignal(&LCDFree);
  LCD_Clear(0);
  LCD_Clear(1);
  
}

/*******************************************************************************
* Set cursor position on LCD display                                           *
*   Parameter:    line  : 0 top, 1 bottom                                      *
*                 column: column position                                      *
*                 line:   line position                                        *
*   Return:                                                                    *
*******************************************************************************/

static void setCursor (long column, long screen)
{
  	unsigned char address;
	if (screen == 0) {   //top screen
		cursor0 = column;
		address = (screen * 40) + column;
		address = 0x80 + (address & 0x7F);
		lcdWriteCmd(address); 	
	} else if (screen == 1) { //bottom screen
		cursor1 = column;
		address = (screen * 40) + column;
		address = 0x80 + (address & 0x7F);
		lcdWriteCmd(address); 
	} else return;               /* Set DDRAM address counter to 0     */
}

/*******************************************************************************
* Set cursor position on LCD display                                           *
*   Parameter:    screen: 0 top, 1 bottom                                      *
*                 column: column position                                      *
*                 line:   line position                                        *
*   Return:                                                                    *
*******************************************************************************/

void LCD_SetCursor (long column, long screen)
{
	OS_bWait(&LCDFree);	
	setCursor(column, screen); 
	OS_bSignal(&LCDFree);
}			 

/*******************************************************************************
* private function Print sting to LCD display                                  *
*   Parameter:    string: pointer to output string                             *
*	  0 for top line, 1 for bottom line. 									   *
*   Return:                                                                    *
*******************************************************************************/
static void print (char *string, long screen)
{
  if(screen == 0) {
    setCursor(cursor0, screen);
  } else if(screen == 1){
    setCursor(cursor1, screen);
  } else return;
  while (*string)  {
    lcdPutchar (*string++);
  }
}

/*******************************************************************************
* Print sting to LCD display                                                   *
*   Parameter:    string: pointer to output string                             *
*	  0 for top line, 1 for bottom line. 										   *
*   Return:                                                                    *
*******************************************************************************/
void LCD_Print (char *string, long screen)
{
  OS_bWait(&LCDFree);
  if(screen == 0) {
    setCursor(cursor0, screen);
  } else if(screen == 1){
    setCursor(cursor1, screen);
  } else return;
  while (*string)  {
    lcdPutchar (*string++);
  }		 
  OS_bSignal(&LCDFree);
}

/*******************************************************************************
* Clear the LCD display                                                        *
*   Parameter: screen: 0 for top line, 1 for bottom line.                      *
*   Return:                                                                    *
*******************************************************************************/

void LCD_Clear (long screen)
{
  OS_bWait(&LCDFree);
  setCursor(0, screen);
  print("                ", screen);
  setCursor(0, screen);
  OS_bSignal(&LCDFree);  
}

/*******************************************************************************
* Print Character to current cursor position                                   *
* Note: Does not increment cursor.                                             *
*   Parameter:    c:      character to be printed                              *
*                 screen: 0 top, 1 bottom                                      *						   
*   Return:                                                                    *
*******************************************************************************/

void LCD_PutChar (char c, long screen)
{ 
  char character[1];
  OS_bWait(&LCDFree);
  character[0] = c;
  print(character, screen);
  OS_bSignal(&LCDFree);
}


/*******************************************************************************
* Print unsigned long to current cursor position                               *
* Note: Does not increment cursor.                                             *
* maximum length of string representing number is 16.						   *
*   Parameter:    output: number to be printed                                 *
*                 screen: 0 top, 1 bottom                                      *						   
*   Return:                                                                    *
*******************************************************************************/
void printUInt(unsigned long output, long screen ) {
	char buffer[16];
	sprintf(buffer, "%-16d", output);
	print(buffer, screen);
}
/*******************************************************************************
* Print unsigned long to current cursor position                               *
* Note: Does not increment cursor.                                             *
* maximum length of string representing number is 16.						   *
*   Parameter:    output: number to be printed                                 *
*                 screen: 0 top, 1 bottom                                      *						   
*   Return:                                                                    *
*******************************************************************************/
void LCD_PrintUInt(unsigned long output, long screen)
{  	
	OS_bWait(&LCDFree);
	printUInt(output, screen);
	OS_bSignal(&LCDFree);
}

/*******************************************************************************
* Print a message including a string and a value							   *
* 	Parameter:		display: 0 top screen, 1 bottom screen					   *
*					pt	: string to be printed								   *	
*					data: number to be printed								   *
*	Return: none															   *
*******************************************************************************/
void LCD_Message(int display, char *pt, unsigned long data){
	int ptlen = strlen(pt);
	LCD_Clear(display);
	OS_bWait(&LCDFree);		// capture resource
	print(pt, display);	// display is 0 or 1, go to first location
	setCursor(ptlen + 1, display);
	printUInt(data, display);

	// we may need to erase characters to end of line

	OS_bSignal(&LCDFree);	// release resource
}


