// filename **********OS.c***********
// Real Time Operating System for Labs 2 and 3, 1/30/10 
// Jonathan W. Valvano 1/30/10, valvano@mail.utexas.edu
// You may use, edit, run or distribute this file 
// You are free to change the syntax/organization of this file

//  Modified by Trang Le and Brendan Long on 1/29/10
//  Date of last revision: 2/26/10
//  Implements blocking semaphores, and priority preepmtive scheduling 
//  TA: Glen Rhodes
//  Purpose: provide access to a 1ms time counter
//  
//  Lab 3
//  Hardware config: TIM 1 is used as 1ms counter, TIM 2 used for periodic task
//					 GPIOB->ODR pin 0 is used as debugging tool for periodic task handler (TIM2)
//					 GPIOB->ODR pin 1 is used as debugging tool for periodic task handler (TIM3)
//					 			pin 2 is used as debugging tool for Systick handler
//					 GPIOB->ODR pin 4-6 is used as debugging tool for outputing ID
//								pin 7 is toggled whenever a thread is blocked
//								pin 8 is toggled whenever a thread is put to sleep
//								pin 9 is toggled whenever a thread is killed
//					 GPIOB->ODR pin 14 is used as debugging tool for PendSV handler

#include <stm32f10x_lib.h>
#include <stdlib.h>

#include "STM32_Init.h"
#include "OS.h"

#define DEBUG 0


#define MAX_TASKS 4
#define STACK_SIZE 512  //best if this is a power of two
#define MAX_FIFO_SIZE	2000
#define RUNLENGTH 30000   // display results and quit when NumSamples==RUNLENGTH

#define  OS_ENTER_CRITICAL()  {CSStart = OS_Time();	cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {									 \
			TimeDisabled = OS_TimeDifference(CSStart, OS_Time());\
			TotalTimeDisabled += TimeDisabled;					 \
			if (TimeDisabled > MaxTimeDisabled)					 \
				MaxTimeDisabled = TimeDisabled;					 \
			OS_CPU_SR_Restore(cpu_sr);}

// PendSV #defines courtesy of uOS/II os_cpu_a.asm
#define NVIC_INT_CTRL 	0xE000ED04		//Interrupt control state register
#define NVIC_PENDSVSET  0x10000000  	//Value to trigger PendSV exception
#define TAMPER_IRQCHANNEL ((u8)0x02)	// Tamper Interrupt

// Definition for TIM1 
#define RCC_APB2ENR_TIM1EN    ((unsigned long)0x00000800)
#define TIM1_UP_IRQChannel    ((u8)0x19)  /* TIM1 Update Interrupt */
#define RCC_APB2ENR_TIM1EN    ((unsigned long)0x00000800)
#define __TIM1_DIER           0x0001                  // 19
#define TIMX_CR1_CEN         ((unsigned short)0x0001)

// Definition for TIM2
#define RCC_APB1ENR_TIM2EN    ((unsigned long)0x00000001)
#define TIM2_IRQChannel       ((u8)0x1C)  /* TIM2 global Interrupt */
#define TIMX_CR1_CEN          ((unsigned short)0x0001)
#define __TIM2_DIER           0x0001                  // 33

// Definition for TIM3
#define TIM3_IRQChannel              ((u8)0x1D)  /* TIM3 global Interrupt */
#define RCC_APB1ENR_TIM3EN    ((unsigned long)0x00000002)
#define __TIM3_DIER               0x0001                  // enable update interrupt

// Definition for EXTI
#define EXTI0_IRQChannel             ((u8)0x06)  /* EXTI Line0 Interrupt */      
#define RCC_APB2ENR_AFIOEN    ((unsigned long)0x00000001)
#define __EXTI_SETUP              1                       //  0
#define __EXTI_USED               0x0001                  //  1
#define __EXTI_INTERRUPTS         0x00000001              //  2
#define __EXTI_IMR                0x00000000              //  3
#define __EXTI_EMR                0x00000000              //  4
#define __EXTI_RTSR               0x00000000              //  5
#define __EXTI_FTSR               0x00000001              //  6
#define __AFIO_EXTICR1            0x00000000              //  7
#define __AFIO_EXTICR2            0x00000000              //  8
#define __AFIO_EXTICR3            0x00000000              //  9
#define __AFIO_EXTICR4            0x00000000              // 10

unsigned long msTime = 0;  // 1ms counter
unsigned long pendSVactive = 0;
unsigned long Stacks[MAX_TASKS][STACK_SIZE];  //allocate space for threads' stacks
unsigned long LastUsedID = 0;
unsigned long mailBox;
unsigned long JitterHistogram1[256]={0,};
unsigned long MaxJitter1 = 0;    // largest time difference between interrupt trigger and running thread
unsigned long MinJitter1= 5000;    // smallest time difference between interrupt trigger and running thread
unsigned long NumSample;   // incremented every sample

static unsigned long CSStart = 0;
static unsigned long TimeDisabled = 0;
static unsigned long MaxTimeDisabled = 0;
static unsigned long TotalTimeDisabled = 0;

/**********************************************************
*					THREAD CONTROL BLOCK			      *
**********************************************************/
struct TCB{
	unsigned long	*StackPt;			// current stack pointer for this thread
	struct TCB 		*Next;				// point to the next TCB
	struct TCB 		*Prev;				// point to the previous TCB
	unsigned long 	Status;				// status of the thread. 
	struct Sema4  *	BlockedPt;
	unsigned long	*StackBase;			// point to the beginning of the thread stack
	unsigned short 	ID;					// unique identifier for the thread (0 to 65535)
	unsigned short  SleepCounter;       // used to suspend execution
	unsigned char 	Priority;			// thread priority (0 is highest)
	//unsigned short  BlockedState;       // used in lab 3
};
typedef struct TCB TCBType;
typedef TCBType *TCBPt;

enum {BLOCKED, SLEEPING, ACTIVE, DEAD};

// We can use an array of TCBTypes to store all the TCBs.
// This way we don't need to use malloc, we can just recycle dead threads.
TCBType Tasks[MAX_TASKS];
TCBPt RunPt = NULL;
TCBPt NextThread = NULL;

void (*PeriodicTask1)(void);	// function pointer for periodic task
void (*PeriodicTask2)(void);  	// function pointer for second periodic task
void (*ButtonTask) (void);		// function pointer for button task
void (*WakeUpTask) (void); 		// function pointer for wake up task

static Sema4Type BoxFree, DataValid, DataAvai, RoomLeft;

struct FIFObuf {
  unsigned long * PutPt;                                // Next In pt
  unsigned long * GetPt;                                // Next Out pt
  unsigned long * buffer;                         		// Buffer
  unsigned long elements;
  unsigned long size;
};
static struct FIFObuf Fifo;

/**********************************************************
*					Critical Section Functions            *
**********************************************************/

//Returns the value of the Interrupt Enable register
__asm unsigned long OS_CPU_SR_Save ( void )  {	
     MRS     R0, PRIMASK                 
     CPSID   I 			  
     BX      LR	
}   

// Modify the CPU_SR to the input. used in conjuction with OS_CPU_SR_Save
__asm void OS_CPU_SR_Restore( unsigned long cpu_st ) {	
	MSR PRIMASK, R0	  
	BX      LR	
}

//******** ContextSwitch **********
// Triggers the PendSV Interrupt
// which will cause the next thread to execute
__asm void ContextSwitch ( void ) {
	LDR R0, =NVIC_INT_CTRL
	LDR R1, =NVIC_PENDSVSET
	STR R1, [R0]
	BX	LR
}

// ******* __WFI *********
// wave form interpolation
__asm void __WFI ( void ) {
	WFI
	BX LR
}

//******** setPriority **********
// Modify the NVIC->IPR priority for the given interrupt
// inputs: interruptNum: the position (table 53 of reference)
//			 of the interrupt e.g TIM2 = 28
// 			priority : the new priority. less is more.
void static setPriority(int interruptNum, char priority){
	int offset = 4 + (8 * (interruptNum % 4));
	NVIC->IPR[interruptNum / 4] |= (priority << offset);
}

// ******** Schedule ************
// Selects the next thread to be run.
// Execution doesn't switch to the new thread
// until PendSV handler runs
void Schedule( void ) {
	unsigned long cpu_sr;
	TCBPt toRun = NULL;
	int highestPriority	= 9999; //very low priority
	TCBPt next = RunPt->Next;

	OS_ENTER_CRITICAL();	
	// iterate through all threads to find highest priority thread
	do {
		if ((next->Status == ACTIVE) && (next->Priority < highestPriority)){
			highestPriority = next->Priority;
			toRun = next;			
		} 
		next = next->Next;
	} while (next != RunPt->Next);		
	NextThread = toRun;

#if DEBUG
	GPIOB->ODR &= ~0x0070;	// clear bit 4-6
	GPIOB->ODR |= ( (OS_Id() << 4) & 0x0070 ); // put the ID into bits 4-6
#endif
	OS_EXIT_CRITICAL();
}

// ********** EXTI0_IRQHandler **************
// irq handler for Wake Up task
// execute every time the Wake Up button is pressed
void EXTI0_IRQHandler(void)
{
  if (EXTI->PR & (1<<0)) {                        // EXTI0 interrupt pending?
    EXTI->PR |= (1<<0);                           // clear pending interrupt
    (*WakeUpTask)();
  }
}

// ******** TIM1_UP_IRQHandler ************
// irq handler for TIM1 UIF interrupt.
// executes once every ms.
void TIM1_UP_IRQHandler (void) {
  unsigned long cpu_sr = 0;
  if ((TIM1->SR & 0x0001) != 0) {                 // check interrupt source
	OS_ENTER_CRITICAL();
	msTime++;
    OS_EXIT_CRITICAL();	
    TIM1->SR &= ~(1<<0);                          // clear UIF flag
  }
} // end TIM1_UP_IRQHandler

// ******** TIM2_IRQHandler ************
// irq handler for TIM2 UIF interrupt.
// executes once every ms.
// toggles GPIOB pin 0 for debugging
void TIM2_IRQHandler (void) {

  static int firstExecution = 1;
  unsigned short thisTime;


  if ((TIM2->SR & 0x0001) != 0) {                 // check interrupt source
  	
	//OS_ENTER_CRITICAL(); 
	thisTime = TIM2->CNT;         	// current time, 1/36 us

#if DEBUG
	GPIOB->ODR |= 0x0001;	
#endif	    
    
    if(!firstExecution){  			// ignore timing of first interrupt
      if(thisTime > MaxJitter1){
        MaxJitter1 = thisTime;
      }
      if(thisTime < MinJitter1){
        MinJitter1 = thisTime;
      }
      if(thisTime<256) JitterHistogram1[thisTime]++; 
    } else { firstExecution = 0; }
	(*PeriodicTask1)();	
    TIM2->SR &= ~(1<<0);                          // clear UIF flag

#if DEBUG
	GPIOB->ODR &= ~0x0001;
#endif

	
	//OS_EXIT_CRITICAL();	
  }
} // end TIM2_UP_IRQHandler

// ******** TIM3_IRQHandler ************
// irq handler for TIM3 UIF interrupt.
// executes once every ms.
// toggles GPIOB pin x for debugging
void TIM3_IRQHandler (void) {

  if ((TIM3->SR & 0x0001) != 0) {                 // check interrupt source
	
#if DEBUG
	GPIOB->ODR |= 0x0002;	
#endif  
	(*PeriodicTask2)();	
    TIM3->SR &= ~(1<<0);                          // clear UIF flag

#if DEBUG
	GPIOB->ODR &= ~0x0002;
#endif
	//OS_EXIT_CRITICAL();	
  }
} // end TIM3_UP_IRQHandler

// ********* TAMPER_IRQHandler **************
// Tamper handler 
// execute when the tamper button is press if enabled
// toggle GPIOB pin 15 for debugging
// copied from keil example      
void TAMPER_IRQHandler(void)
{   
  if (BKP->CSR & (1<<9) ) {                       // Tamper interrupt flag
    PWR->CR  |=  (1<<8);                          // enable access to RTC, BDC registers
    BKP->CSR |=  (1<<1);                          // clear Tamper Interrupt
	BKP->CSR |=  (1<<0);                          // clear tamper Event
    PWR->CR  &= ~(1<<8);                          // disable access to RTC, BDC registers
	if ((BKP->DR1 == 0) && (BKP->DR2 == 0)  )
	    {	                     	    	
			(*ButtonTask)(); 
		}
	}
}

// ******** SysTick_Handler ************
//  SysTick interrupt happens every TIMESLICE
//  Scedules the next thread to execute and triggers a context switch
// 	toggling pin GPIOB bit 1 for debugging
void SysTick_Handler (void) { 
	int i;

#if DEBUG
	GPIOB->ODR |= 0x0004;
#endif
// update Sleep Counter here
	for (i=0; i<MAX_TASKS; i++){
		if( Tasks[i].Status == SLEEPING ) {
           // OS_ENTER_CRITICAL();   don't need to disable...sleeping thread isn't running, 
                                    //and threads only affect their own sleep state.
			Tasks[i].SleepCounter--;  
            if( Tasks[i].SleepCounter == 0 ){  //done sleeping?
                Tasks[i].Status =   ACTIVE;   //wake up!
#if DEBUG
				GPIOB->ODR |= 0x0200; //toggle bit 9
				GPIOB->ODR &= ~0x0200; //toggle bit 9
#endif
			}
			//OS_EXIT_CRITICAL();
        }
	}	
    Schedule();		  //choose the next thread to run
	ContextSwitch();  //trigger PendSV_Handler
		
#if DEBUG
	GPIOB->ODR &= ~0x0004;
#endif
} // end SysTick_Handler

// ******** PendSV_Handler ************
// Switches out foreground tasks.
// derived from PendSV_Handler documentation uOS/II
// note: the first context switch is a special case: 
// we don't save registers from the context that initializes
// the threads.
// toggling pin GPIOB bit 2 for debugging
__asm void PendSV_Handler (void) {
	;CPU saved xPSR, PC, LR, R12, R0-R3
    EXTERN RunPt
	EXTERN NextThread
    EXTERN pendSVactive
	CPSID   I                                   ; Prevent interruption during context switch

;DEBUGGING INSTRUMENT at port GPIOB->ODR
#if DEBUG == 1
	LDR		R0, =0x40010C0C						; port B starts at 0x4001 0C00 from datasheet
	LDR		R1, [R0]							; port B ODR has offset 0x0C
	ORR		R1, #0x4000
	STR		R1, [R0]		
#endif
//    
    LDR     R0, =pendSVactive                   ; Determine if this is the first context switch
    LDR     R0, [R0]
    CBZ     R0, OS_CPU_PendSVHandler_nosave     ; Skip register save the first time
    
    MRS     R0, MSP                             ; load R0 with the stack pt
    SUBS    R0, R0, #0x20                       ; Save remaining regs r4-11 on process stack
    STM     R0, {R4-R11}

    LDR     R1, =RunPt                          ; RunPt->StackPt = SP;      
    LDR     R1, [R1]
    STR     R0, [R1]                            ; R0 is SP of process being switched out
                                                ; At this point, entire context of process has been saved
OS_CPU_PendSVHandler_nosave
    LDR     R0, =pendSVactive				    ; pendSVactive = 1    //no longer the first context switch
    MOV     R1, #1					
    STR     R1, [R0]
    
    ;RunPt = NextThread                        
	LDR     R0, =RunPt                          ; R0 = location of RunPt
    LDR     R1, =NextThread						; R0 = location of NextThread
    LDR     R2, [R1]							; R2 = value of NextThread
    STR     R2, [R0]							; store value of NextThread into address of RunPt

    LDR     R0, [R2]                            ; R0 is new process SP; SP = RunPt->StackPt;
    LDM     R0, {R4-R11}                        ; Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20
    MSR     MSP, R0                             ; Load MSP with new process SP
    ;ORR     LR, LR, #0x04                      ; Ensure exception return uses process stack

;DEBUGGING INSTRUMENT
#if DEBUG == 1
	LDR		R0, =0x40010C0C						; port B starts at 0x4001 0C00 from datasheet
	LDR		R1, [R0]							; port B ODR has offset 0x0C
	BIC		R1, #0x4000
	STR		R1, [R0]		
#endif

    CPSIE   I									; Enable interrupts again
    BX      LR                                  ; return will restore remaining context
}

// ******** OS_Init ************
// initialize operating system, disable interrupts until OS_Launch 
// input:  none
// output: none
void OS_Init(void){
	int i;
	__disable_irq();//disable interrupts
	for( i = 0; i < MAX_TASKS; i++) {
		Tasks[i].Status = DEAD;
	}	  	
} 

// ******** OS_Time ************
// reads the current time
// Inputs:  none
// Outputs: time in 1/36 us units, 0 to 35999
// The time resolution should be at least 1us, and the precision at least 14 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_TimeDifference have the same resolution and precision 
unsigned short OS_Time(void){
	return TIM1->CNT;
}

// ******** OS_ClearMsTime ************
// sets the system time to zero from Lab 1)
// Inputs:  none
// Outputs: none
// You are free to change how this works
void OS_ClearMsTime(void){
	msTime = 0;
}

// ******** OS_MsTime ************
// reads the current time in msec (from Lab 1)
// Inputs:  none
// Outputs: time in ms units
unsigned long OS_MsTime(void){
	return msTime;
}

// ******** OS_InitSemaphore ************
// initialize semaphore 
// input:  pointer to a semaphore
// output: none
void OS_InitSemaphore(Sema4Type *semaPt, long value){
	semaPt->Value = value;		//this isn't a critical section, is it?
} 

// ******** OS_Wait ************
// decrement semaphore and spin/block if less than zero
// input:  pointer to a counting semaphore
// output: none
void OS_Wait(Sema4Type *semaPt){
	unsigned long cpu_sr;
	TCBPt thisThread = RunPt;
    OS_ENTER_CRITICAL();
	semaPt->Value--;   // -- is a read-modify-write
	
    if (semaPt->Value < 0)
	{	
		// block this thread
	   	thisThread->Status = BLOCKED;	// set the status of this thread to be blocked
#if DEBUG
		GPIOB->ODR |= 0x0080;	//toggle bit 8
		GPIOB->ODR &= ~0x0080;	//toggle bit 8
#endif
		thisThread->BlockedPt = semaPt;
		OS_Suspend(); 		// we want to suspend after restoring the I bit right?...or in the if statement?
	}
	OS_EXIT_CRITICAL();
} 

// ******** OS_Signal ************
// increment semaphore, wakeup blocked thread if appropriate 
// input:  pointer to a counting semaphore
// output: none
void OS_Signal(Sema4Type *semaPt){
	unsigned long cpu_sr;
	TCBPt toWake = NULL;
	int highestPriority = 5000;	  // a really low value
	
	TCBPt iterator = RunPt;
	OS_ENTER_CRITICAL();
	semaPt->Value++;
	if (semaPt->Value <= 0)
	{
		do {	  //find the highest priority blocked thread to wake up
			if((iterator->BlockedPt == semaPt)&&(iterator->Status == BLOCKED)) {
				if(iterator->Priority < highestPriority) 
				{
					highestPriority = iterator->Priority;
					toWake = iterator;
				}
			}
            iterator = iterator->Next;
		}while(iterator != RunPt);
		
		if(toWake == NULL){
			OS_EXIT_CRITICAL();
			return;
		}
		toWake->Status = ACTIVE;
#if DEBUG
		GPIOB->ODR |= 0x0200;
		GPIOB->ODR &= ~0x0200;
#endif
		toWake->BlockedPt = NULL;		     	
	}
	OS_EXIT_CRITICAL();	 
} 

// ******** OS_bWait ************
// if the semaphore is 0 then spin/block
// if the semaphore is 1, then clear semaphore to 0
// input:  pointer to a binary semaphore
// output: none
void OS_bWait(Sema4Type *semaPt){
 	OS_Wait(semaPt);
} 

// ******** OS_bSignal ************
// set semaphore to 1, wakeup blocked thread if appropriate 
// input:  pointer to a binary semaphore
// output: none
void OS_bSignal(Sema4Type *semaPt){
	OS_Signal(semaPt);
} 

// ******** StackInit ************
// derived from uOS/II OSTaskStkInit function
// initializes a stack frame for a task being added.
// initial values of registers is unimportant, so we put
// special values into them to aid debugging
// Inputs: stack pointer
// 		   pointer to a function 
// Outputs: stack pointer pointing at the end of the stack
unsigned long * StackInit(unsigned long * stk, void(*task)(void)) {
	*(stk)   = 0x01000000L; /* xPSR */
	*(--stk) = (unsigned long)task; /* Entry Point */
	*(--stk) = 0xFFFFFFFEL; /* R14 (LR) */
	*(--stk) = 0x12121212L; /* R12 */
	*(--stk) = 0x03030303L; /* R3 */
	*(--stk) = 0x02020202L; /* R2 */
	*(--stk) = 0x01010101L; /* R1 */
	*(--stk) = 0;//p_arg; /* R0 : argument */
	/* Remaining registers saved on process stack */
	*(--stk) = 0x11111111; /* R11 */
	*(--stk) = 0x10101010L; /* R10 */
	*(--stk) = 0x09090909L; /* R9 */
	*(--stk) = 0x08080808L; /* R8 */
	*(--stk) = 0x07070707L; /* R7 */
	*(--stk) = 0x06060606L; /* R6 */
	*(--stk) = 0x05050505L; /* R5 */
	*(--stk) = 0x04040404L; /* R4 */
	return stk;
}

//******** OS_AddThread *************** 
// add a foregound thread to the scheduler
// Inputs: pointer to a void/void foreground task
//         number of bytes allocated for its stack
//         priority (0 is highest)
// Outputs: 1 if successful, 0 if this thread can not be added
// stack size must be divisable by 8 (aligned to double word boundary)
// In Lab 2, you can ignore both the stackSize and priority fields
// In Lab 3, you can ignore the stackSize fields
int OS_AddThread(void(*task)(void), 
unsigned long stackSize, unsigned long priority){
	unsigned long cpu_sr;
	TCBPt newThread = NULL;
	int i = 0;
	TCBPt LinkedListIterator = RunPt;
	unsigned long * initialStack = NULL;
	
	(void)stackSize;  //ignore this for lab 3. Every thread will have the same stackSize.
		
	//loop through Tasks, find dead thread to convert into this new one, 
	//newThread pointing to recycleable thread
	for( i = 0; i < MAX_TASKS; i++) {
		OS_ENTER_CRITICAL();
		if( Tasks[i].Status == DEAD ) { //Dead thread. We can recycle
			newThread = &Tasks[i];
			newThread->Status = ACTIVE;
			OS_EXIT_CRITICAL();
			break;  //we've got our thread
		}
		OS_EXIT_CRITICAL();		
	}	
	if ( newThread == NULL ) return 0;  //no room for more threads. return error code.
	newThread->ID = ++LastUsedID;		// unique identifier for the thread (0 to 65535)
	newThread->SleepCounter = 0; 		// 0 means not asleep
	newThread->Priority = priority;		// thread priority (0 is highest)
	initialStack = StackInit(&Stacks[i][127], task);//prepare the new thread's stack
    newThread->StackPt = initialStack;				// current stack pointer for this thread
   	newThread->StackBase = &Stacks[i][127];			// point to the largest number of the thread stack (stack grows up)
#if DEBUG	
	GPIOB->ODR |= 0x0200;
	GPIOB->ODR &= ~0x0200;
#endif
		
	//insert the thread into the linked list
	if( RunPt == NULL ) {  //this is the first thread added
		OS_ENTER_CRITICAL();
		RunPt = newThread;
        NextThread = newThread; 
		newThread->Next = newThread;
		newThread->Prev = newThread;  //a circle of length one
		OS_EXIT_CRITICAL();
	} else {
		while( LinkedListIterator->Next != RunPt) { //Loop until LLIterator points at last thread
			LinkedListIterator = LinkedListIterator->Next;
		}
		OS_ENTER_CRITICAL();
		newThread->Next = LinkedListIterator->Next;
		newThread->Prev = LinkedListIterator;
		LinkedListIterator->Next->Prev = newThread;
		LinkedListIterator->Next = newThread;
		OS_EXIT_CRITICAL();
	}
	
	return 1;															 	
}

//******** OS_Id *************** 
// returns the thread ID for the currently running thread
// Inputs: none
// Outputs: Thread ID, number greater than zero 
unsigned long OS_Id(void){
	return RunPt->ID;
}

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
// First time called, OS will enable TIM2
// Second time called, OS will enable TIM3
int OS_AddPeriodicThread(void(*task)(void), 
	unsigned long period, unsigned long priority){
	unsigned long cpu_sr;

    	
	// error checking. Only 2 periodic threads can be active
	if (task == NULL || 
	 ( (PeriodicTask1 != NULL) && (PeriodicTask2 != NULL))){
		return 0; // this thread cannot be added
	}
	OS_ENTER_CRITICAL();
	if ( PeriodicTask1 == NULL ) { //adding the first periodic task
		// enable TIM2 interrupt and have its handler call task
		PeriodicTask1 = task;
	
		// enable TIM2
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;                 // enable clock for TIM2	
		TIM2->PSC = 15;            							// set prescaler
		TIM2->ARR = period/16-1;       							// set auto-reload	
		TIM2->CR1 = 0;                                      // reset command register 1
		TIM2->CR2 = 0;                                      // reset command register 2		
		TIM2->DIER = __TIM2_DIER;                           // enable interrupt
		NVIC->ISER[0]  = (1 << (TIM2_IRQChannel & 0x1F));   // enable interrupt		
		TIM2->CR1 |= TIMX_CR1_CEN;                          // enable timer
	  	setPriority(28, (char)priority) ;   					// set priority for TIM2
		// end TIM2 used		
	} else {	
		// enable TIM3 interrupt and have its handler call task
		PeriodicTask2 = task;
	
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;                 // enable clock for TIM3
	    TIM3->PSC = 15;           							// set prescaler
	    TIM3->ARR = period/16-1;            						// set auto-reload
	    TIM3->CR1 = 0;                                      // reset command register 1
	    TIM3->CR2 = 0;                                      // reset command register 2
	    TIM3->DIER = __TIM3_DIER;                           // enable interrupt
	    NVIC->ISER[0]  = (1 << (TIM3_IRQChannel & 0x1F));   // enable interrupt
	    TIM3->CR1 |= TIMX_CR1_CEN;                          // enable timer
		setPriority(29, (char)priority) ;  					//set priority for TIM3
	}  // end TIM3 used
	OS_EXIT_CRITICAL();
    return 1; 
}

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
int OS_AddButtonTask(void(*task)(void), unsigned long priority){
	if (task == NULL)
		return 0; //this thread cannot be added
	// enable TAMP and have its handler do task
	ButtonTask = task;
	BKP->CSR |= 0x00000004;
	NVIC->ISER[0] = (1 << (TAMPER_IRQCHANNEL & 0x1F));	// enable Tamper interrupt
	setPriority(2, (char)priority);						//  set priority
	return 1;
}

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
// Using PA0 falling edge
int OS_AddWakeupTask(void(*task)(void), unsigned long priority){
	if (task == NULL)
		return 0;	// this thread cannot be added
	WakeUpTask = task;

//	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
//    AFIO->EXTICR[0] &= 0xFFF0;                              // clear used pin
//    AFIO->EXTICR[0] |= (0x000F & __AFIO_EXTICR1);           // set pin to use
//    EXTI->IMR       |= ((1 << 0) & __EXTI_IMR);             // unmask interrupt
//    EXTI->EMR       |= ((1 << 0) & __EXTI_EMR);             // unmask event
//    EXTI->RTSR      |= ((1 << 0) & __EXTI_RTSR);            // set rising edge
//    EXTI->FTSR      |= ((1 << 0) & __EXTI_FTSR);            // set falling edge
//    NVIC->ISER[0]  = (1 << (EXTI0_IRQChannel & 0x1F));    	// enable interrupt EXTI 0
	setPriority(6, (char)priority);			// set priority
	return 1;	  
}

// ********* OS_DisableWakeUpTask ********
// OS feature to disarm Wake Up interrupt
// input: none
// output: none
void OS_DisableWakeUpTask(void){
	NVIC->ICER[0]  = (1 << (EXTI0_IRQChannel & 0x1F));    	// disable interrupt EXTI 0
}

// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of system tick interrupts to sleep
// output: none
// You are free to select the time resolution for this function
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(unsigned long sleepTime){
	RunPt->SleepCounter = sleepTime;
	RunPt->Status = SLEEPING;
#if DEBUG
	GPIOB->ODR |= 0x0100;
	GPIOB->ODR &= ~0x0100;
#endif
	OS_Suspend(); // go to sleep, switch the next thread
} 

// ******** OS_Kill ************
// kill the currently running thread, release its TCB memory
// input:  none
// output: none
void OS_Kill(void){
	unsigned long cpu_sr;

	RunPt->Status = DEAD;
#if DEBUG
	GPIOB->ODR |= 0x0400;
	GPIOB->ODR &= ~0x0400;
#endif
	Schedule();
	OS_ENTER_CRITICAL();
	// remove the running thread from linked list by updating the pointers
	RunPt->Prev->Next = RunPt->Next;
	RunPt->Next->Prev = RunPt->Prev;	
	// both pointers are null
	RunPt->Next = NULL;
	RunPt->Prev = NULL;	
	OS_EXIT_CRITICAL();

	ContextSwitch();
}

// ******** OS_Suspend ************
// suspend execution of currently running thread
// scheduler will choose another thread to execute
// Can be used to implement cooperative multitasking 
// Same function as OS_Sleep(0)
// input:  none
// output: none
void OS_Suspend(void){
	Schedule();
	ContextSwitch();   
}
 
// ******** OS_Fifo_Init ************
// Initialize the Fifo to be empty
// Inputs: size
// Outputs: none 
// In Lab 2, you can ignore the size field
// In Lab 3, you should implement the user-defined fifo size
// In Lab 3, you can put whatever restrictions you want on size
//    e.g., 4 to 64 elements
//    e.g., must be a power of 2, 4,8,16,32,64,128
void OS_Fifo_Init(unsigned long size){
	if(size > MAX_FIFO_SIZE) return;
    if( Fifo.buffer != NULL ) { //reinitializing. must free old buffer
        free(Fifo.buffer);
    }
//	Fifo.buffer = array;
    Fifo.buffer = malloc(size * sizeof(unsigned long));  //dynamic allocation of buffer.
    Fifo.size = size;
    Fifo.PutPt = Fifo.GetPt = &Fifo.buffer[0];
	Fifo.elements = 0;
	OS_InitSemaphore(&DataAvai, 0);
	OS_InitSemaphore(&RoomLeft, size);	
 }

// ******** OS_Fifo_Put ************
// Enter one data sample into the Fifo
// Called from the background, so no waiting 
// Inputs:  data
// Outputs: true if data is properly saved,
//          false if data not saved, because it was full
// Since this is called by interrupt handlers 
//  this function can not disable or enable interrupts
int OS_Fifo_Put(unsigned long data){
	unsigned long * tempPt;
	tempPt = Fifo.PutPt;
	tempPt++;
	if(tempPt==&Fifo.buffer[Fifo.size]){ //wrap if necessary
		tempPt = &Fifo.buffer[0];
	}

	// full
	if(tempPt == Fifo.GetPt) {
		return 0;
	} else {
		*(Fifo.PutPt) = data; //put
		Fifo.PutPt = tempPt;
		Fifo.elements++;
		OS_Signal(&DataAvai);
		return 1;
	} 		  
} 

// ******** OS_Fifo_Get ************
// Remove one data sample from the Fifo
// Called in foreground, will spin/block if empty
// Inputs:  none
// Outputs: data 
unsigned long OS_Fifo_Get(void){
	unsigned long data;

	// check if the fifo is empty
	OS_Wait(&DataAvai);
	data = *(Fifo.GetPt++);
	if (Fifo.GetPt == &(Fifo.buffer[Fifo.size])) {
		Fifo.GetPt = &(Fifo.buffer[0]);
	}
	Fifo.elements--;
	return data;	   
}

// ******** OS_Fifo_Size ************
// Check the status of the Fifo
// Inputs: none
// Outputs: returns the number of elements in the Fifo
//          greater than zero if a call to OS_Fifo_Get will return right away
//          zero or less than zero if the Fifo is empty 
//          zero or less than zero  if a call to OS_Fifo_Get will spin or block
long OS_Fifo_Size(void){
	return Fifo.elements;	 
}

// ******** OS_MailBox_Init ************
// Initialize communication channel
// Inputs:  none
// Outputs: none
void OS_MailBox_Init(void){
	OS_InitSemaphore(&BoxFree, 1);
	OS_InitSemaphore(&DataValid, 0); 
	mailBox = 0;
}

// ******** OS_MailBox_Send ************
// enter mail into the MailBox
// Inputs:  data to be sent
// Outputs: none
// This function will be called from a foreground thread
// It will spin/block if the MailBox contains data not yet received 
void OS_MailBox_Send(unsigned long data){
	OS_bWait(&BoxFree);
	// put data into mailbox
	mailBox = data;
	OS_bSignal(&DataValid);
}

// ******** OS_MailBox_Recv ************
// remove mail from the MailBox
// Inputs:  none
// Outputs: data received
// This function will be called from a foreground thread
// It will spin/block if the MailBox is empty 
unsigned long OS_MailBox_Recv(void){
	unsigned long data;

	OS_bWait(&DataValid);
	// retrieve data from Mailbox
	data = mailBox;
	OS_bSignal(&BoxFree);
	return data;	
}


// ******** OS_TimeDifference ************
// Calculates difference between two times
// Inputs:  two times measured with OS_Time
// Outputs: time difference in 1/36 us units
// The time resolution should be at least 1us, and the precision at least 14 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_Time have the same resolution and precision 
unsigned short OS_TimeDifference(unsigned short start, unsigned short stop){
	return stop - start;
}


// ******** OS_MaxTimeDisabled ************
// Returns the Maximum period of time
// that interrupts are disabled
unsigned long OS_MaxTimeDisabled ( void ) {
	return MaxTimeDisabled;
}

unsigned long OS_TotalTimeDisabled (void){
	return TotalTimeDisabled;
}

// ******** OS_PercentTimeDisabled ************
// Returns the percent of time
// that interrupts are disabled
// returns percentage as whole number between 0 and 100.
unsigned long OS_PercentTimeDisabled ( void ) {
	unsigned long timeRunning = (OS_MsTime() * 1000 * 36) + OS_Time(); //in 1/36 us
	return (TotalTimeDisabled * 10000) / timeRunning;		
}


// ******** IdleTask **********
// dummy thread to run when all other threads are 
// unrunnable
void IdleTask ( void ) {
	for(;;){
		//__wfi(); //disable clock to processor to save power
	}
}

//******** OS_Launch *************** 
// start the scheduler, enable interrupts
// Inputs: number of clock cycles for each time slice
//         you may select the units of this parameter
// Outputs: none (does not return)
// In Lab 2, you can ignore the theTimeSlice field
// In Lab 3, you should implement the user-defined theTimeSlice field
void OS_Launch(unsigned long theTimeSlice){

    OS_AddThread(&IdleTask, 128, 9999); //put in the idle task. Very low priority

	ContextSwitch();	//prepare for the first thread to execute		 	
    SCB->SHPR[2] = 0xE0F00000;		   // set the priority for Systick and PendSV as the lowest
    SysTick->CTRL |= 0x00000007;       //set up SysTick to handle thread swapping
    SysTick->LOAD = theTimeSlice;
    __enable_irq();
    for(;;);
}


