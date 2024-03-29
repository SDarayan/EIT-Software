/*----------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: 'Retarget' layer for target-dependent low level functions
 * Version: V1.0
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <rt_misc.h>
#include "Serial.h"
#pragma import(__use_no_semihosting_swi)


extern short Serial_SendChar (char c);
extern char Serial_GetChar (void);


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;
FILE __stderr;

int fputc (int ch, FILE *f) { return (Serial_SendChar(ch)); }

int fgetc (FILE *f)         { return (Serial_GetChar()); }


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int ch)       { Serial_SendChar(ch); }


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}
