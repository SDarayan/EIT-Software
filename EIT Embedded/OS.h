// filename **********OS.H***********
// Real Time Operating System for Labs 2 and 3 
// Jonathan W. Valvano 2/7/10, valvano@mail.utexas.edu
// You may use, edit, run or distribute this file 
// You are free to change the syntax/organization of this file
 
//  Modified by Trang Le and Brendan Long on 1/29/10
//  Date of last revision: 2/22/10
//  Implements spin-lock semaphores, and round-robin preepmtive scheduling 
//  TA: Glen Rhodes
//  Purpose: provide access to a 1ms time counter
//  
//  Lab 2
//  Hardware config: TIM 1 is used as 1ms counter, TIM 2 used for periodic task

#ifndef __OS_H
#define __OS_H  1

#define TIME_1MS  72000        // fill this in so it means 1 ms
#define TIME_2MS  2*TIME_1MS       


/**********************************************************
*					Profile data                          *
**********************************************************/
struct Profile {
	unsigned long msTime;
	unsigned long counterTick;
	unsigned long event;
	unsigned long threadID;
}; 
typedef struct Profile Profiling;
#define MAX_PROFILES 100


Profiling * OS_GetProfileData( void );
void OS_Clear_Profile_Data( void ); 

/**********************************************************
*					Critical Section Functions            *
**********************************************************/
// Disable/Enable interrupts by preserving the state of interrupts. 
// Code copied from uOS/II source
//note that every function that uses these functions must allocate cpu_sr
void OS_CPU_SR_Restore ( unsigned long );
unsigned long OS_CPU_SR_Save( void );



__asm void __WFI ( void ) ;




/**********************************************************
*					SEMAPHORE DATA						  *
**********************************************************/
// feel free to change the type of semaphore, there are lots of good solutions
struct  Sema4{
	long Value;   // >0 means free, <0 means busy
};
typedef struct Sema4 Sema4Type;

     
// ******** OS_Init ************
// initialize operating system, disable interrupts until OS_Launch 
// input:  none
// output: none
void OS_Init(void); 

// ******** OS_InitSemaphore ************
// initialize semaphore 
// input:  pointer to a semaphore
// output: none
void OS_InitSemaphore(Sema4Type *semaPt, long value); 

// ******** OS_Wait ************
// decrement semaphore and spin/block if less than zero
// input:  pointer to a counting semaphore
// output: none
void OS_Wait(Sema4Type *semaPt); 

// ******** OS_Signal ************
// increment semaphore, wakeup blocked thread if appropriate 
// input:  pointer to a counting semaphore
// output: none
void OS_Signal(Sema4Type *semaPt); 

// ******** OS_bWait ************
// if the semaphore is 0 then spin/block
// if the semaphore is 1, then clear semaphore to 0
// input:  pointer to a binary semaphore
// output: none
void OS_bWait(Sema4Type *semaPt); 

// ******** OS_bSignal ************
// set semaphore to 1, wakeup blocked thread if appropriate 
// input:  pointer to a binary semaphore
// output: none
void OS_bSignal(Sema4Type *semaPt); 

//******** OS_AddThread *************** 
// add a foregound thread to the scheduler
// Inputs: pointer to a void/void foreground task
//         number of bytes allocated for its stack
//         priority (0 is highest)
// Outputs: Thread ID if successful, 0 if this thread can not be added
// stack size must be divisable by 8 (aligned to double word boundary)
// In Lab 2, you can ignore both the stackSize and priority fields
// In Lab 3, you can ignore the stackSize fields
int OS_AddThread(void(*task)(void), 
   unsigned long stackSize, unsigned long priority);

//******** OS_Id *************** 
// returns the thread ID for the currently running thread
// Inputs: none
// Outputs: Thread ID, number greater than zero 
unsigned long OS_Id(void);

//******** OS_AddPeriodicTask *************** 
// add a background periodic task to the scheduler
// this function will receive highest priority
// Inputs: pointer to a void/void background function
//         period given in system time units
//         priority 0 is highest, 12 is lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// It is assumed that the user task will run to completion and return
// This task can not spin, block, loop, sleep, or kill
// This task can call OS_Signal  OS_bSignal	 OS_AddThread
// You are free to select the time resolution for this function
// This task does not have a Thread ID
// In lab 2, this command will be called 0 or 1 times
// In lab 2, the priority field can be ignored
// In lab 3, this command will be called 0 1 or 2 times
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
int OS_AddPeriodicThread(void(*task)(void), 
   unsigned long period, unsigned long priority);

//******** OS_AddButtonTask *************** 
// add a background task to run whenever the TAMP button is pushed
// Inputs: pointer to a void/void background function
//         priority 0 is highest, 12 is lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// It is assumed that the user task will run to completion and return
// This task can not spin, block, loop, sleep, or kill
// This task can call OS_Signal  OS_bSignal	 OS_AddThread
// This task does not have a Thread ID
// In labs 2 and 3, this command will be called 0 or 1 times
// In lab 2, the priority field can be ignored
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
int OS_AddButtonTask(void(*task)(void), unsigned long priority);

//******** OS_AddWakeupTask *************** 
// add a background task to run whenever the WKUP button is pushed
// Inputs: pointer to a void/void background function
//         priority 0 is highest, 12 is lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// It is assumed user task will run to completion and return
// This task can not spin block loop sleep or kill
// It can call issue OS_Signal, it can call OS_AddThread
// This task does not have a Thread ID
// In lab 2, this function can be ignored
// In lab 3, this command will be called will be called 0 or 1 times
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
int OS_AddWakeupTask(void(*task)(void), unsigned long priority);

// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of system tick interrupts to sleep
// output: none
// You are free to select the time resolution for this function
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(unsigned long sleepTime); 

// ******** OS_Kill ************
// kill the currently running thread, release its TCB memory
// input:  none
// output: none
void OS_Kill(void); 

// ******** OS_Suspend ************
// suspend execution of currently running thread
// scheduler will choose another thread to execute
// Can be used to implement cooperative multitasking 
// Same function as OS_Sleep(0)
// input:  none
// output: none
void OS_Suspend(void);
 
// ******** OS_Fifo_Init ************
// Initialize the Fifo to be empty
// Inputs: size
// Outputs: none 
// In Lab 2, you can ignore the size field
// In Lab 3, you should implement the user-defined fifo size
// In Lab 3, you can put whatever restrictions you want on size
//    e.g., 4 to 64 elements
//    e.g., must be a power of 2, 4,8,16,32,64,128
void OS_Fifo_Init(unsigned long size);

// ******** OS_Fifo_Put ************
// Enter one data sample into the Fifo
// Called from the background, so no waiting 
// Inputs:  data
// Outputs: true if data is properly saved,
//          false if data not saved, because it was full
// Since this is called by interrupt handlers 
//  this function can not disable or enable interrupts
int OS_Fifo_Put(unsigned long data);  

// ******** OS_Fifo_Get ************
// Remove one data sample from the Fifo
// Called in foreground, will spin/block if empty
// Inputs:  none
// Outputs: data 
unsigned long OS_Fifo_Get(void);

// ******** OS_Fifo_Size ************
// Check the status of the Fifo
// Inputs: none
// Outputs: returns the number of elements in the Fifo
//          greater than zero if a call to OS_Fifo_Get will return right away
//          zero or less than zero if the Fifo is empty 
//          zero or less than zero  if a call to OS_Fifo_Get will spin or block
long OS_Fifo_Size(void);

// ******** OS_MailBox_Init ************
// Initialize communication channel
// Inputs:  none
// Outputs: none
void OS_MailBox_Init(void);

// ******** OS_MailBox_Send ************
// enter mail into the MailBox
// Inputs:  data to be sent
// Outputs: none
// This function will be called from a foreground thread
// It will spin/block if the MailBox contains data not yet received 
void OS_MailBox_Send(unsigned long data);

// ******** OS_MailBox_Recv ************
// remove mail from the MailBox
// Inputs:  none
// Outputs: data received
// This function will be called from a foreground thread
// It will spin/block if the MailBox is empty 
unsigned long OS_MailBox_Recv(void);

// ******** OS_Time ************
// reads the current time
// Inputs:  none
// Outputs: time in 1/36 us units, 0 to 35999
// The time resolution should be at least 1us, and the precision at least 14 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_TimeDifference have the same resolution and precision 
unsigned short OS_Time(void);

// ******** OS_TimeDifference ************
// Calculates difference between two times
// Inputs:  two times measured with OS_Time
// Outputs: time difference in 1/36 us units
// The time resolution should be at least 1us, and the precision at least 14 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_Time have the same resolution and precision 
unsigned short OS_TimeDifference(unsigned short start, unsigned short stop);

// ******** OS_ClearMsTime ************
// sets the system time to zero from Lab 1)
// Inputs:  none
// Outputs: none
// You are free to change how this works
void OS_ClearMsTime(void);

// ******** OS_MsTime ************
// reads the current time in msec (from Lab 1)
// Inputs:  none
// Outputs: time in ms units
// You are free to select the time resolution for this function
unsigned long OS_MsTime(void);


// ******** OS_Init ************
// initialize operating system, disable interrupts until OS_Launch 
// input:  none
// output: none
void OS_Init(void); 


// ******** OS_MaxTimeDisabled ************
// Returns the Maximum period of time
// that interrupts are disabled
unsigned long OS_MaxTimeDisabled ( void ) ;

// ******** OS_PercentTimeDisabled ************
// Returns the percent of time
// that interrupts are disabled
// returns percentage as whole number between 0 and 100.
unsigned long OS_PercentTimeDisabled ( void ) ;

unsigned long OS_TotalTimeDisabled( void );

//******** OS_Launch *************** 
// start the scheduler, enable interrupts
// Inputs: number of clock cycles for each time slice
//         you may select the units of this parameter
// Outputs: none (does not return)
// In Lab 2, you can ignore the theTimeSlice field
// In Lab 3, you should implement the user-defined theTimeSlice field
void OS_Launch(unsigned long theTimeSlice);

#endif


