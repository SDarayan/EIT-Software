//-----StimPattern.c-------
// functions to implement a stimulation pattern
// Built on top of Switching.c, so top level application
// should not need to access Switching directly.

// Pattern 2 operates as follows:  ABSOLUTE
// for 16 electrodes, labeled 0 - 15, 
// current is injected across 16 pairs of electrodes, one pair at a time.
// The first injection is across 0 and 8, then 1 and 9, up to 15 and 7.
// For each pair of current electrodes, voltage is measured across pairs of adjacent electrodes.
// Voltage measurement moves clockwise around the ring, starting at the electrode next to the source electrode.
// So for current electrodes 0 and 8, voltage is measured at (1, 2), (2, 3) up to (6,7) skip 8, (9, 10), on up to 
// v1 begins at source + 1.



#include "Switching.h"
#include <stdio.h>

static unsigned short source, sink, v1, v2;
static unsigned short progress = 0;
static int Pattern;

#define PATTERN_ABSOLUTE 2
#define PATTERN_DIFFERENCE 1

//---- StimPattern_Print -----
// displays the current values of source, sink, v1, v2
void StimPattern_Print( void ){
	printf("%3d |  src: %2d | snk: %2d | v1: %2d | v2: %2d\n\r", progress, source, sink, v1, v2);
}

//------- StimPattern_Progress ------
// returns the number of configurations that have occured in this pattern.
// starts at 0 and usually goes up to about 200
// inputs - none
// outputs - the progress so far
unsigned short StimPattern_Progress( void ){
	return progress;
}


	

//----- StimPatternInit ------
// configures the electrodes for initial measurement
// inputs: pattern is a number that specifies which pattern to use. this must match up 
// with what the pc expects
//outputs: 0 on success, 1 means you messed up
int StimPattern_Init( int pattern ){
	Pattern = pattern;
	if(Pattern == 2){
		progress = 0;
		source = 0;
		sink = 8;
		v1 = 1;   
		v2 = 2;
		Switch_CurrentInjection(source, sink);
		Switch_VoltageMeasurement(v1, v2);
		Switch_EnableCurrent();
		Switch_EnableVoltageMeasurement();
		return 0;
	} else if (Pattern == 1){
		progress = 0;
		source = 0;
		sink = 1;
		v1 = 2;
		v2 = 3;
		Switch_CurrentInjection(source, sink);
		Switch_VoltageMeasurement(v1, v2);
		Switch_EnableCurrent();
		Switch_EnableVoltageMeasurement();
		return 0;
	}
	else return 1;
}

//increments the current electrodes and adjusts the voltage electrodes
//should not be called from the last electrode
static void newCurrentElectrodes( void ){
	if(Pattern == 2){
		source = (source + 1) % 16;
		sink   = (sink   + 1) % 16;
	} else if(Pattern == 1){
		source = (source + 1) % 16;
		sink   = (sink   + 1) % 16;
	}
}

// StimPattern_Next
// returns 0 if there is more stuff in the pattern
// returns 1 if all of the elements of the pattern have
// already been output
int StimPattern_Next( void ){		   	
	if(Pattern == PATTERN_ABSOLUTE){
		progress++;	 		
	    if( (v2 + 1)%16 == sink){ //skip the current electrodes
			v2 = (sink + 2) % 16; //move extra to skip sink.
			v1 = (sink + 1)% 16;
		} else if( ((v2+1)%16) == source ){ //are we done with this current setup?
			//printf("v2+1 % 16 = %d", ((v2+1)%16));
			newCurrentElectrodes();
			v1 = (source + 1) % 16;
			v2 = (source + 2) % 16;
			if( source == 0 ) { //we've hit the start from v2 == source... must be done
				return 1;
			}
			Switch_CurrentInjection(source, sink);
		} else {	//normal update
			v1 = v2;
			v2 = (v2+1)%16;
		}
		Switch_VoltageMeasurement(v1, v2);
		return 0;
	} else if(Pattern == PATTERN_DIFFERENCE){
	  progress++;
	  v1 = v2;
	  v2 = (v2+1)%16;
	  if(v2 == source){
	  	newCurrentElectrodes();
        Switch_CurrentInjection(source, sink);
		if(source == 0){
			return 1;
		}
		v1 = (sink + 1)%16;
		v2 = (sink + 2)%16;
	  }
      Switch_VoltageMeasurement(v1, v2);
	  return 0;
	}
	return 1;
}
