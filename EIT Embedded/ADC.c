/*----------------------------------------------------------------------------
 * Name:    ADC.h
 * Purpose: prototypes for basic input functions using ADC.
 * Created by: Trang Le, Brendan Long on 01/27/2010
 * Lab 1
 * TA: Glen Rhodes
 * Date of last revision: 01/27/2010
 * Hardware Configuration: 
  PINS: PA = GPIOA = ADC1
 *	knob pot resistor at ADC1 channel 1.
 * verified for channels 1-3.   
 *---------------------------------------------------------------------------*/
/*******************************************************************************
* ADC_SR bits                                                                  *
*******************************************************************************/
#define STRT  0x00000010 //Bit 4: Regular channel Start flag 
#define JSTRT 0x00000008 //Bit 3 JSTRT: Injected channel Start flag
#define JEOC  0x00000004 //Bit 2 JEOC: Injected channel end of conversion
#define EOC   0x00000002 //Bit 1 EOC: End of conversion
#define AWD   0x00000001 //Bit 0 AWD: Analog watchdog flag
/*******************************************************************************
* ADC_CR1 bits                                                                 *
*******************************************************************************/
#define AWDEN   0x00800000 //Bit 23 AWDEN: Analog watchdog enable on regular channels
#define JAWDEN  0x00400000 //Bit 22 JAWDEN: Analog watchdog enable on injected channels
#define JDISCEN 0x00001000 //Bit 12 JDISCEN: Discontinuous mode on injected channels
#define DISCEN  0x00000800 //Bit 11 DISCEN: Discontinuous mode on regular channels
#define JAUTO   0x00000400 //Bit 10 JAUTO: Automatic Injected Group conversion
#define AWDSGL  0x00000200 //Bit 9 AWDSGL: Enable the watchdog on a single channel in scan mode
#define SCAN    0x00000100 //Bit 8 SCAN: Scan mode
#define JEOCIE  0x00000080 //Bit 7 JEOCIE: Interrupt enable for injected channels
#define AWDIE   0x00000040 //Bit 6 AWDIE: Analog watchdog interrupt enable
#define EOCIE   0x00000020 //Bit 5 EOCIE: Interrupt enable for EOC
/*******************************************************************************
* ADC_CR2																	   *
*******************************************************************************/
#define TSVREFE  0x00800000 //Bit 23 TSVREFE: Temperature sensor and VREFINT enable
#define SWSTART  0x00400000 //Bit 22 SWSTART: Start conversion of regular channels
#define JSWSTART 0x00200000 //Bit 21 JSWSTART: Start conversion of injected channels
#define EXTTRIG  0x00100000 //Bit 20 EXTTRIG: External trigger conversion mode for regular channels
#define JEXTRIG  0x00008000	//Bit 15 JEXTTRIG: External trigger conversion mode for injected channels	 
#define ALIGN    0x00000800 //Bit 11 ALIGN: Data alignment
#define DMA      0x00000100 //Bit 8 DMA: Direct memory access mode
#define RSTCAL   0x00000008 //Bit 3 RSTCAL: Reset calibration
#define CAL      0x00000004 //Bit 2 CAL: A/D Calibration
#define CONT     0x00000002 //Bit 1 CONT: Continuous conversion
#define ADON     0x00000001 //Bit 0 ADON: A/D converter ON / OFF

#include <stm32f10x_lib.h>	 // STM32F10x Library Definitions
#include "ADC.h"

/*******************************************************************************
* Initialize ADC                                                               *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/
void ADC_Open(void)
{
	GPIOA->CRL &= ~0x00000FFF; // set PIN1, PIN2, PIN3 as analog input (see stm32_Init)
	RCC->APB2ENR |= (1<<9);                         // enable periperal clock for ADC1
	ADC1->SQR1  = 0x00000000;                       // only one conversion
  	ADC1->SMPR2 = 0x00000B68;                       // set sample time channel1, 2, 3 (55,5 cycles)
  	
	ADC1->CR1   = 0x00000000;                       // use independant mode, SCAN mode off
	
}

/*******************************************************************************
* Perform analog to digital conversion                                         *
*   Parameter:  channel: specify analog channel to sample, 1 2 or 3            *
*   Return: ADC sample                                                         *
*******************************************************************************/

unsigned long ADC_In( long channel)
{
	ADC1->SQR3  = channel;//0x00000001;                       // set channel1 as 1st conversion
	ADC1->CR2 = ADON;  //Turn on A/D converter	
	ADC1->CR2 |= ADON; //Start conversion
    while(!( ADC1->SR & EOC)){}
	return ADC1->DR;
}



