/*----------------------------------------------------------------------------
 * Name:    Interpreter.h
 * Purpose: command line interface with public functions of the 
 * 			lcd, adc, and timer features for lab 1.
 * Created by:  Brendan Long on 02/1/2010
 * Date of last revision: 03/11/2010
 * Hardware Configuration: 
 *	knob pot resistor at ADC1 channel 1.
 * verified for channels 1-3.   
 *---------------------------------------------------------------------------*/



//Interpreter_interp
//inputs none
// returns: null
// provides command line interface to RTOS for lab 2
// toggles GPIOB if DEBUG enabled
void Menu_interp (void);

