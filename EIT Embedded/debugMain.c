//debug main

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

#define TIMESLICE 10 * TIME_1MS

/*----------------------------------------------------------------------------
  Debugging main function
 *----------------------------------------------------------------------------*/

void ADC_Sampler ( void ) 
{   
	char message[16];
	unsigned short adcRaw;
	unsigned short adcWhole;
	unsigned short adcDec;
	for(;;)
	{
		adcRaw = ADC_In(1);
		adcWhole = adcRaw / 1241;
		adcDec = ((adcRaw - (1241 * adcWhole)) * 100)  / 1241;
		
		adcRaw = ADC_In(2);
		adcWhole = adcRaw / 1241;
		adcDec = ((adcRaw - (1241 * adcWhole)) * 100)  / 1241;
		OS_Sleep(1);
	}
}
	
			    
 int  debugMain (void) 
 {
   Switch_Init(0, 1, 2, 3);
   stm32_Init(); 
   Serial_Open();                                  // init RX / TX buffers
   OS_Init();
   ADC_Open();
   
   OS_AddThread(&ADC_Sampler, 128, 2);	
   OS_AddThread(&Menu_interp, 128, 1);
   OS_Launch(TIMESLICE);
}  

