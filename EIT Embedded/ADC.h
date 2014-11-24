/*----------------------------------------------------------------------------
 * Name:    ADC.h
 * Purpose: prototypes for basic input functions using ADC.
 * Created by: Trang Le, Brendan Long on 01/27/2010
 * Lab 1
 * TA: Glen Rhodes
 * Date of last revision: 01/27/2010
 * Hardware Configuration: 
  PINS:    
 *---------------------------------------------------------------------------*/

/*******************************************************************************
* Initialize ADC                                                               *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/
void ADC_Open(void);

/*******************************************************************************
* Perform analog to digital conversion                                         *
*   Parameter:  channel: specify analog channel to sample, 1 2 or 3            *
*   Return: ADC sample                                                         *
*******************************************************************************/

unsigned long ADC_In(long channel);



