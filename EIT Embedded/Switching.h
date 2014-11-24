//  ****** Switch.c *****
// Public Functions for switching selected electrodes
// Spring 2010 Senior design project
// 16 pins on GPIOC used for data selection
// pins 0 - 3 : select Current Source 1 - 16
// pins 4 - 7 : select Current Sink 1 - 16 (must be different than current source
// pins 8 - 11 : select Voltage Measurement 1
// pins 12 - 15 : select Voltage Measurement 2 
// Note that the VoltageMeasurement electrodes should be 
// switched more often than the current electrodes because of the 
// setup time for the signal to propagate through the measurand.

// GPIOB pins 12 - 15 used for output enable of decoders.
// pins 12 : Current Source OE
// pins 13 : Current Sink OE
// pins 14 : Voltage Measurement 1 OE
// pins 15 : Voltage Measurement 2 OE

#ifndef __SWITCHING_H
#define __SWITCHING_H 1

//******* Switch_Init ****************
// set up the switching to disable outputs
void Switch_Init( unsigned short source, unsigned short sink, 
	 				unsigned short v1, unsigned short v2);

//******* Switch_DisableVoltageMeasurement ************
// Turns off output enable signals on signal selector circuits.
void Switch_DisableVoltageMeasurement ( void ) ;

//******* Switch_EnableVoltageMeasurement ************
// Turns on output enable signals on signal selector circuits.
void Switch_EnableVoltageMeasurement ( void ) ;

//******* Switch_DisableCurrent ************
// Turns off output enable signals on signal selector circuits.
void Switch_DisableCurrent ( void ) ;

//******* Switch_EnableCurrent ************
// Turns on output enable signals on signal selector circuits.
void Switch_EnableCurrent ( void ) ;

//******* Switch_EnableAllElectrodes ************
// Turns on output enable signals on signal selector circuits.
void Switch_DisableAllElectrodes ( void ) ;

//******* Switch_CurrentInjection **********
// source and sink cannot be the same electrode
// returns 0 on error
int Switch_CurrentInjection ( unsigned short source, unsigned short sink ) ;

//****** Switch_VoltageMeasurement **********
// v1 and v2 are electrodes selected to measure a differential voltage
// they cannot be the same electrode.
// returns 0 on error
int Switch_VoltageMeasurement ( unsigned short v1, unsigned short v2 ) ;

#endif 
