/*----------------------------------------------------------------------------
 * Name:    main.c
 * Purpose: embedded software to operate EIT hardware.
 * 			Spring 2010 EE464k Senior design project.
 *			This project coordinates the stimulation pattern for an EIT measurement system.
 * 			It samples an ADC channel and sends data via serial port to a PC
 * Author: Brendan Long. Created on April 1, 2010
 * 			Modified from Blinky project in Keil example code.
 * 
 * Version: V1.00
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stm32f10x_lib.h>                        // STM32F10x Library Definitions
#include <string.h>

#include "STM32_Init.h"                           // STM32 Initialization
#include "LCD.h"                                  // LCD function prototypes
#include "ADC.h"                                  // ADC funtion prototypes
#include "Serial.h"      
#include "Switching.h" 
#include "menu.h"                    
#include "StimPattern.h"  
#include "OS.h"           

#define DEBUG 0				//Determines which main to use, hardware debug = 0, normal operation = 1
#define ADC_CHANNEL	3		//Determines which ADC channel we're measureing

#define PATTERN_ABSOLUTE 2
#define PATTERN_DIFFERENCE 1

/***************************/
/*		COMMAND CODES      */
/***************************/

#define PING 					'p'	//sends	DATA_FRAME_LENGTH bytes of all 1s
#define PING_ADC 				'g'	//sends DATA_FRAME_LENGTH bytes. Each pair is high/low of an ADC sample
#define SIMULATION 				's' //sends TestData array, one element at a time.

//sends DATA_FRAME_LEGNTH bytes (the first 192 are real data). each pair is an ADC sample from absolute stim pattern
#define EIT_ABSOLUTE 			'a'
//sends DATA_FRAME_LEGNTH bytes (the first 208 are real data). each pair is an ADC sample from difference stim pattern
#define EIT_DIFFERENCE 			'd'

#define EIT_ABSOLUTE_STORED   	'A' // just like 'a', but stores result until an 'R' is received
#define EIT_DIFFERENCE_STORED 	'D' // just like 'd', but stores result until an 'R' is received
#define RETRIEVE_MEASUREMENT  	'R' //sends whatever is in StoredMeasurements

//Streaming data commands.
//usage: 
//		1) OPEN_STREAM[1 or 2]  //1 is absolute, 2 is difference. You get one now, more when you call NEXT
//		2)	STREAM_NEXT			// get a frame instantly. Another is stored for later.
//		3)  CLOSE_STREAM		// end the streaming.
#define OPEN_STREAM1			'1' //will make the mcu output difference continuously until STOP command
#define OPEN_STREAM2			'2' //will make the mcu output absolute continuously until STOP command
#define STREAM_NEXT				'N'	//promts the microcontroller for the next frame. Starts another sample
#define CLOSE_STREAM			'C' //closes the stream (all this really does is clear out the stored data



#define DATA_FRAME_LENGTH 512
#define TIMESLICE 5 * TIME_1MS
#define USING_LCD 0 

//Simulated data = should create known image for testing comm.
//Two entries make one sample. 1st entry is high bits, 2nd entry is low bits.
// 
unsigned char TestData[416] = 
{15, 42, 6, 141, 3, 228, 2, 198, 2, 60, 1, 252, 1, 232, 1, 250, 2, 60, 2,
202, 3, 242, 6, 160, 15, 87, 15, 59, 6, 166, 3, 253, 2, 214, 2, 70, 1, 254,
1, 230, 1, 247, 2, 57, 2, 198, 3, 230, 6, 151, 15, 68, 15, 42, 15, 93, 6,
192, 4, 11, 2, 225, 2, 77, 2, 5, 1, 239, 2, 3, 2, 68, 2, 201, 3, 226, 6,
126, 6, 141, 15, 59, 15, 90, 6, 180, 4, 11, 2, 226, 2, 82, 2, 15, 1, 251, 2,
13, 2, 72, 2, 201, 3, 220, 3, 228, 6, 165, 15, 93, 15, 58, 6, 181, 4, 10, 2,
230, 2, 87, 2, 20, 1, 253, 2, 9, 2, 66, 2, 194, 2, 197, 3, 253, 6, 192, 15,
89, 15, 55, 6, 177, 4, 13, 2, 231, 2, 88, 2, 16, 1, 245, 2, 2, 2, 59, 2, 60,
2, 214, 4, 11, 6, 180, 15, 58, 15, 83, 6, 190, 4, 12, 2, 230, 2, 83, 2, 10,
1, 241, 1, 254, 1, 252, 2, 70, 2, 225, 4, 11, 6, 181, 15, 55, 15, 82, 6,
176, 4, 9, 2, 226, 2, 82, 2, 12, 1, 241, 1, 232, 1, 255, 2, 77, 2, 226, 4,
10, 6, 177, 15, 83, 15, 54, 6, 179, 4, 9, 2, 229, 2, 84, 2, 9, 1, 250, 1,
229, 2, 5, 2, 82, 2, 230, 4, 13, 6, 190, 15, 82, 15, 54, 6, 177, 4, 13, 2,
229, 2, 77, 2, 60, 1, 247, 1, 239, 2, 15, 2, 88, 2, 231, 4, 12, 6, 176, 15,
53, 15, 84, 6, 191, 4, 12, 2, 221, 2, 202, 2, 57, 2, 3, 1, 251, 2, 20, 2,
88, 2, 230, 4, 9, 6, 179, 15, 54, 15, 86, 6, 179, 4, 3, 3, 242, 2, 198, 2,
68, 2, 13, 1, 253, 2, 16, 2, 83, 2, 226, 4, 9, 6, 177, 15, 84, 15, 61, 6,
178, 6, 160, 3, 230, 2, 201, 2, 72, 2, 9, 1, 245, 2, 10, 2, 82, 2, 230, 4,
13, 6, 191, 15, 86, 15, 65, 15, 87, 6, 151, 3, 226, 2, 201, 2, 66, 2, 2, 1,
241, 2, 12, 2, 84, 2, 229, 4, 12, 6, 178, 15, 61, 15, 68, 6, 126, 3, 220, 2,
194, 2, 59, 1, 254, 1, 241, 2, 9, 2, 77, 2, 221, 4, 3, 6, 178, 15, 66}; 

#define MAX_MEASUREMENTS 416
unsigned char StoredMeasurements[MAX_MEASUREMENTS]={0,};
unsigned short MeasurementHalves = 0; 

int Pattern;
int StoringData = 0;


Sema4Type Idling; 


void StoreMeasurement(unsigned char measurement)
{
	StoredMeasurements[MeasurementHalves++] = measurement;
}	

//-------------------- THREAD FUNCTIONS --------------------------

void Ping ( void ) {
	int i;
#if USING_LCD
	LCD_Print("Ping", 1);
#endif
	for(i = 0; i < DATA_FRAME_LENGTH; i++)
	{
		Serial_SendChar(0xFF);
	}
	OS_Signal(&Idling);
	OS_Kill();
}

void PingADC ( void ) {
	int i;
	unsigned short adcSample;
	unsigned char high, low;
#if USING_LCD
    LCD_Print("Sending ADC samples", 1);
#endif
	for(i = 0; i < DATA_FRAME_LENGTH / 2; i++)
	{
		adcSample = ADC_In(2);
		LCD_PrintUInt(adcSample, 1);
				
		high = (adcSample & 0x0000FF00) >> 8;
		low = (adcSample & 0x000000FF);
		Serial_SendChar(high);
		Serial_SendChar(low);
	}
	OS_Signal(&Idling);
	OS_Kill();
}

void Simulation ( void ){
	int i, dataSent = 0;
#if USING_LCD
	LCD_Print("Sending test data...", 0);
#endif
	for(i = 0; i < 416; i+=2)
	{
		Serial_SendChar(TestData[i]); //send high bits
		Serial_SendChar(TestData[i+1]);	 //send low bits
		dataSent += 2;
	}
	//pad the transmission frame
	while(dataSent < DATA_FRAME_LENGTH){
		Serial_SendChar(0);
		dataSent++;
	}
#if USING_LCD
	LCD_Print("Transfer complete!", 1);
#endif
	OS_Signal(&Idling);
	OS_Kill();
}		



//-------- EIT_FrameMeasurement----------
//Thread that controls electodes and ADC 
//under normal operation
// used for either absolute or difference EIT stim. patterns, 
// depending on 'Pattern' variable
// sends a frame of 'DATA_ 
void EIT_FrameMeasurement ( void ) {
	unsigned short adcSample;
	unsigned char high, low;  //used to split adcSample into two bytes.
	unsigned long runTime = 0;
	int dataSent = 0;	
	OS_ClearMsTime();
	StimPattern_Init(Pattern);
	do {
		OS_Sleep(1);
		adcSample = ADC_In(ADC_CHANNEL);
		
		if(StoringData == 1)
		{
			high = (adcSample & 0xFF00) >> 8;
			StoreMeasurement(high);
			low = (adcSample & 0x00FF);	
			StoreMeasurement(low);
		} else
		{
			high = (adcSample & 0xFF00) >> 8;
			Serial_SendChar(high);
			low = (adcSample & 0x00FF);			
			Serial_SendChar(low);
		}
			
		dataSent += 2;
				
	} while(!StimPattern_Next());

	if(StoringData == 0) {
		//pad the data fram
		while(dataSent < DATA_FRAME_LENGTH){
			Serial_SendChar(0);
			dataSent++;
		}		   
	}
	runTime = OS_MsTime();
#if USING_LCD	
    LCD_Message(1, "time (ms): ", runTime); 
#endif
	OS_Signal(&Idling);
	return;
}

//Destructive Read of the Stored Data.
//Sends all data in StoredMeasurements 
// to the serial channel.
// destructive read so we don't send junk the next time around
void Transmit_Measurements ( void ) {
	int i;
	for(i = 0; i < MAX_MEASUREMENTS; i++)
	{
		Serial_SendChar(StoredMeasurements[i]);
		StoredMeasurements[i] = 0;
	}
	MeasurementHalves = 0;
	while(i < DATA_FRAME_LENGTH)
	{
		Serial_SendChar(0);
		i++;
	}
	OS_Signal(&Idling);
	return;
}

void CommandReceiver( void ){
	unsigned char input;
	while(1)
	{
		input = Serial_GetChar();
#if USING_LCD		
        LCD_Print("Received: ", 0);
		LCD_SetCursor(10, 0);
		LCD_PutChar(input, 0);
		LCD_SetCursor(0,0);
#endif
		switch(input)
		{
			case PING:				
				OS_Wait(&Idling);
				OS_AddThread(&Ping, 128, 1);
				break;
			case PING_ADC:				
				OS_Wait(&Idling);
				OS_AddThread(&PingADC, 128, 1);
				break;
			case SIMULATION:
				Simulation();
				OS_Wait(&Idling);
				OS_AddThread(&Simulation, 128, 1);
				break;
			case EIT_ABSOLUTE:
				OS_Wait(&Idling);
				Pattern = 2;
				StoringData = 0;
				EIT_FrameMeasurement();
				break;
			case EIT_DIFFERENCE:
				OS_Wait(&Idling);
				Pattern = 1;
				StoringData = 0;
				EIT_FrameMeasurement();
				break;
			case EIT_ABSOLUTE_STORED:
				OS_Wait(&Idling);
				Pattern = 2;
				StoringData = 1;
				EIT_FrameMeasurement();
				break;
			case EIT_DIFFERENCE_STORED:
				OS_Wait(&Idling);
				Pattern = 1;
				StoringData = 1;
				EIT_FrameMeasurement();
				break;
			case RETRIEVE_MEASUREMENT:
				OS_Wait(&Idling);
				Transmit_Measurements();
				break;					
			case OPEN_STREAM1:			 //difference
				OS_Wait(&Idling);				
				Pattern = PATTERN_DIFFERENCE;
				StoringData = 0;
				EIT_FrameMeasurement(); //stream out the first frame
				OS_Wait(&Idling);
				StoringData = 1;
				EIT_FrameMeasurement(); //store for when STREAM_NEXT is called
				break;
			case OPEN_STREAM2:			 //absolute
				OS_Wait(&Idling);				
				Pattern = PATTERN_ABSOLUTE;
				StoringData = 0;
				EIT_FrameMeasurement(); //stream out the first frame
				OS_Wait(&Idling);
				StoringData = 1;
				EIT_FrameMeasurement(); //store for when STREAM_NEXT is called
				break;				
		   case STREAM_NEXT:
		   		OS_Wait(&Idling);
				Transmit_Measurements();				
				OS_Wait(&Idling);
				StoringData = 1;
				EIT_FrameMeasurement(); //store for when STREAM_NEXT is called
 				break;
			case CLOSE_STREAM:
				OS_Wait(&Idling);
				for(MeasurementHalves = 0; MeasurementHalves < MAX_MEASUREMENTS; MeasurementHalves++)
				{
					StoredMeasurements[MeasurementHalves] = 0;
				}
				MeasurementHalves = 0;
				OS_Signal(&Idling);
				break;				
		}
	}
}

 
 /*----------------------------------------------------------------------------
  Actual main function
 *----------------------------------------------------------------------------*/
 int main (void) {
   	stm32_Init (); 
   	OS_Init();
   	
   	Serial_Open();                                  // init RX / TX buffers
   	ADC_Open();
#if USING_LCD    
    LCD_Init();
   	LCD_Clear(0);
  	LCD_Clear(1);
   	LCD_Print("Awaiting command...", 0);
#endif   
   		
	OS_InitSemaphore(&Idling, 1);
	OS_AddThread(&CommandReceiver, 128, 1);
	OS_Launch(TIMESLICE);
} 


