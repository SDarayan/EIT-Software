
//---- StimPattern_Print -----
// displays the current values of source, sink, v1, v2
void StimPattern_Print( void );


//------- StimPattern_Progress ------
// returns the number of configurations that have occured in this pattern.
// starts at 0 and usually goes up to about 200
// inputs - none
// outputs - the progress so far
unsigned short StimPattern_Progress( void );

//----- StimPatternInit ------
// configures the electrodes for initial measurement
// inputs: pattern is a number that specifies which pattern to use. this must match up 
// with what the pc expects
//outputs: 0 on success, 1 means you messed up
int StimPattern_Init( int pattern );


// StimPattern_Next
// returns 0 if there is more stuff in the pattern
// returns 1 if all of the elements of the pattern have
// already been output
int StimPattern_Next( void );

