//  ****** Switch.c *****
// Public Functions for switching selected electrodes
// Spring 2010 Senior design project

// 2 pins on GPIOB used for Output enable
// pin 1 : Voltage measurement enable
// pin 0 : Current injection enable

// 8 pins on GPIOB used for data selection
// pins 15-12 : select Voltage 2
// pins 11-8  : select Voltage 1

// 8 pins of GPIOC used for current selections
// pins 3-0 : sink electrode number
// pins 7-4 : source electrode number



// Note that the VoltageMeasurement electrodes should be 
// switched more often than the current electrodes because of the 
// setup time for the signal to propagate through the measurand.

#include <stm32f10x_lib.h> 
#include "Switching.h"

#define MAX_ELECTRODES 16



//******* Switch_Init ****************
// set up the switching to disable outputs
// and select electrodes 1 and 2 for current 
// and 3 and 4 for voltage
void Switch_Init( unsigned short source, unsigned short sink, 
	 				unsigned short v1, unsigned short v2) {

	Switch_CurrentInjection ( source, sink );
	Switch_VoltageMeasurement ( v1, v2 );

}

//******* Switch_DisableVoltageMeasurement ************
// Turns off output enable signals on signal selector circuits.
void Switch_DisableVoltageMeasurement ( void ) {
	GPIOB->ODR &= ~0x0002;  // switch off Port B pin 0
}

//******* Switch_EnableVoltageMeasurement ************
// Turns on output enable signals on signal selector circuits.
void Switch_EnableVoltageMeasurement ( void ) {
	GPIOB->ODR |= 0x0002;  // switch on Port B pin 0
}

//******* Switch_DisableCurrent ************
// Turns off output enable signals on signal selector circuits.
void Switch_DisableCurrent ( void ) {
	GPIOB->ODR &= ~0x0001;  // switch off Port B pin 1
}

//******* Switch_EnableCurrent ************
// Turns on output enable signals on signal selector circuits.
void Switch_EnableCurrent ( void ) {
	GPIOB->ODR |= 0x0001;  // switch on Port B pin 1
}

//******* Switch_CurrentInjection **********
// source and sink cannot be the same electrode
// returns 0 on error
int Switch_CurrentInjection ( unsigned short source, unsigned short sink ) {
   if ( ( source == sink ) || (source >= MAX_ELECTRODES) || (sink >= MAX_ELECTRODES) )
   		return 0; 

	
	GPIOC->ODR &= ~0x00FF; // clear selected current electrodes
	GPIOC->ODR |= ((source << 4) & 0x00F0); // select source
	GPIOC->ODR |= ((sink  << 0) & 0x000F);

	return 1;
}

//****** Switch_VoltageMeasurement **********
// v1 and v2 are electrodes selected to measure a differential voltage
// they cannot be the same electrode.
// returns 0 on error
int Switch_VoltageMeasurement ( unsigned short v1, unsigned short v2 ) {
	if ( ( v1 == v2 ) || (v1 >= MAX_ELECTRODES) || (v2 >= MAX_ELECTRODES) ) 
		return 0;
	
	
	GPIOB->ODR &= ~0xFF00; // clear selected voltage electrodes
	GPIOB->ODR |= (v1  << 8) & 0x0F00 ;  // select v1
	GPIOB->ODR |= (v2  << 12) & 0xF000;	// select v2
	return 1;	
}
