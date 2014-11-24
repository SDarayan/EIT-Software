/*----------------------------------------------------------------------------
 * Name:    Interpreter.c
 * Purpose: command line interface with public functions of the 
 * 			Stimulation Patterns, and electrode switching
 * Created by: Trang Le, Brendan Long on 02/1/2010
 * Modified from EE345M lab 1.
 * EE464k Senior design project
 * TA: Sanmi Koyejo
 * Date of last revision: 04/1/2010
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <stm32f10x_lib.h>                        // STM32F10x Library Definitions
#include <string.h>


#include "Serial.h"
#include "Switching.h"
#include "StimPattern.h"
#include "ADC.h"

//Interpreter_interp
//inputs none
// returns: null
// provides command line interface to RTOS for lab 2
// toggles GPIOB if DEBUG enabled
void Menu_interp (void) {
	char input[16];
	char *token = NULL;
	char delims[] = " ";
	char * arg1 = NULL;
	char * arg2 = NULL;
	unsigned short adcSample;
	unsigned short numSamples;
	int electrode1, electrode2, pattern;
	unsigned short adcWhole, adcDec;
	int i,j;
	
	for (;;){		 
		
		printf("\r\nType 'cur [src] [snk]' to change the source/sink\r\n");
		printf("Type 'vol [v1] [v2]' to change the active measurement electrodes/sink\r\n");
		printf("Type 'dv' 'ev' 'dc' 'ec' to disable/enable voltage or current electrodes\n\r");
		printf("Type 'nxt' to output the next configuration in the sequence\n\r");
		printf("Type 'full [pattern]' to output a full sequence. pattern 1: difference 208, pattern2: absolute 192.\n\r");
		printf("Type 'end' to run the sequence to completion\n\r");
		printf("Type 'adc [num] to sample some\n\r");
		Serial_GetLine(input, 16);
		printf("\n\r");
		token = strtok( input, delims );
		arg1 = strtok( NULL, delims );
		arg2 = strtok( NULL, delims );
	   	if ( (!strcmp(token, "cur")) && (arg1 != NULL) && (arg2 != NULL) ){
			sscanf(arg1, "%d", &electrode1);
			sscanf(arg2, "%d", &electrode2);
			printf("change to current electrodes %d %d\r\n", electrode1, electrode2);
			if( !Switch_CurrentInjection( (u16)electrode1, (u16) electrode2 )){
				printf("There was a problem...\n\r");
			}
		} else if ( (!strcmp(token, "vol")) && (arg1 != NULL) && (arg2 != NULL) ){
			sscanf(arg1, "%d", &electrode1);
			sscanf(arg2, "%d", &electrode2);
			printf("change to measurement electrodes %d %d\r\n", electrode1, electrode2);
			if( !Switch_VoltageMeasurement( (u16)electrode1, (u16) electrode2 )){
				printf("There was a problem switching the voltage electrodes");
			}
		} else if ( (!strcmp(token, "dv") )) {
			Switch_DisableVoltageMeasurement();
		} else if ( (!strcmp(token, "ev") )) {
			Switch_EnableVoltageMeasurement();
		} else if ( (!strcmp(token, "dc") )) {
			Switch_DisableCurrent();
		} else if ( (!strcmp(token, "ec") )) {
			Switch_EnableCurrent();
		} else if ( (!strcmp(token, "nxt") )) {
			if( StimPattern_Next() ) {
				printf("You reached the end of the sequence");
			} else {
				StimPattern_Print();
			}
		} else if ( (!strcmp(token, "end") ) ){
			while(StimPattern_Next());
			printf("Total length of sequence: %d\n\r", StimPattern_Progress());
		}else if ( (!strcmp(token, "full") ) ){
			sscanf(arg1, "%d", &pattern);
			StimPattern_Init(pattern);
			StimPattern_Print();
			while( ! StimPattern_Next()){
				StimPattern_Print();
			}
			printf("Sequence complete.\n\r");				
		} else if ((!strcmp(token, "adc"))) {
			sscanf(arg1, "%d", &numSamples);
			for(i = 0; i < numSamples; i++)
			{
				adcSample = ADC_In(3);
				adcWhole = (adcSample * 330) / 4095;
				printf("%4d\n\r", adcWhole);
				//delay
				for( j = 0; j < 10000; j++)
				{
					adcSample++;
				}
			}	
		}		
		else printf("unknown command\n\r");		   		
	}											   
}


