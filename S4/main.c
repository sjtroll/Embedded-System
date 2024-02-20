/**
 ******************************************************************************
 * @file    repo/s4/main.c
 * @author  Sejin Son
 * @date    10/09/23
 * @brief   FreeRtos with console (Stage 4)
 ******************************************************************************
 *
 */
#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "s4451119_sysmon.h"
#include "semphr.h"
#include "debug_log.h"
#include "s4451119_joystick.h"
#include <string.h>
#include "s4451119_ascsys.h"


// Task Priorities (Idle Priority is the lowest priority)
#define TASK1_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define TASK2_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define TASK3_PRIORITY					( tskIDLE_PRIORITY + 2 )
#define CONSOLE_PRIORITY				( tskIDLE_PRIORITY + 2 )


// Task Stack Allocations (must be a multiple of the minimal stack size)
#define TASK1_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define TASK2_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define TASK3_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define CONSOLE_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )


void Task1_Task( void );
void Task2_Task( void );
void Task3_Task( void );
void consoleTask(void *pvParameters);
void s4451119_semaphore_pb();


// Semaphore for joystick Z button
SemaphoreHandle_t s4451119SemaphorePb;	
// Handle for Task 2
TaskHandle_t task2Handle = NULL;	
// Queue for Task 3, used to send signals to consoleTask
QueueHandle_t xQueue;

static int previousTime = 0;


/*
 * Starts all the other tasks, then starts the scheduler. Must not have a Cyclic Executive.
 */
int main( void ) {
	
	s4451119SemaphorePb = xSemaphoreCreateBinary();
	xQueue = xQueueCreate(10, sizeof(int));
	
	HAL_Init();		//Only HAL_Init() must be called before task creation.
	
	
	//
	//Only functions that create tasks must be called in main.
	//

	// Start the task to flash the LED.
    xTaskCreate( (void *) &Task1_Task, (const signed char *) "T1", TASK1_STACK_SIZE, NULL, TASK1_PRIORITY, NULL);
	xTaskCreate( (void *) &Task2_Task, (const signed char *) "T2", TASK2_STACK_SIZE, NULL, TASK2_PRIORITY, &task2Handle);
	xTaskCreate( (void *) &Task3_Task, (const signed char *) "T3", TASK3_STACK_SIZE, NULL, TASK3_PRIORITY, NULL);
	xTaskCreate((TaskFunction_t) consoleTask, "Console Task", CONSOLE_STACK_SIZE, NULL, CONSOLE_PRIORITY, NULL);



	/* Start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */

	vTaskStartScheduler();

  	return 0;
}

/*
 * Task 1:
 * - Initializes debug UART and system monitor.
 * - Toggles the green LED in an infinite loop.
 * - Sets and clears the logic analyzer (LA) Channel 0 during each iteration.
 */
void Task1_Task( void ) {
	BRD_debuguart_init();
	s4451119_sysmon_init();
	S4451119_REG_SYSMON_CHAN0_CLR();				//Clear LA Channel 0
	
	for (;;) {

		
		S4451119_REG_SYSMON_CHAN0_SET();			//Set LA Channel 0	
		BRD_LEDGreenToggle();		//Random instruction	

		vTaskDelay(3);			
		S4451119_REG_SYSMON_CHAN0_CLR();			//Clear LA Channel 0	
        
	}
}

/*
 * Task 2: LED Blue Toggle, LA Channel 1 Control & Queue Communication
 * - Clears the LA Channel 1 initially for clean measurements.
 * - In an infinite loop:
 *   1. Enters a critical section to ensure atomic operations and prevent preemption.
 *   2. Sets LA Channel 1, indicating the start of an event.
 *   3. Toggles the blue LED, providing a visual sign of task execution.
 *   4. Delays the task to simulate some work.
 *   5. Clears LA Channel 1, marking the end of an event.
 *   6. Exits the critical section.
 *   7. Sends a signal to the consoleTask via a queue, indicating Task 2 has completed its loop iteration.
 */
void Task2_Task( void ) {
	
	S4451119_REG_SYSMON_CHAN1_CLR();			//Clear LA Channel 1

	for (;;) {

        taskENTER_CRITICAL();
        S4451119_REG_SYSMON_CHAN1_SET();    // Set LA Channel 1    
        BRD_LEDBlueToggle();    // Random instruction    
	    vTaskDelay(10);
        S4451119_REG_SYSMON_CHAN1_CLR();    // Clear LA Channel 1
		taskEXIT_CRITICAL(); 

		 // Send a signal/message to the consoleTask through the queue.
        int signal = 1;  // Arbitrary value representing the signal.
        xQueueSend(xQueue, &signal, portMAX_DELAY);  // Send the signal, waiting indefinitely if the queue is full.
		


	}
}

/*
 * Task 3: LED Red Toggle, Joystick Button Management & Task 2 Control
 * - Initializes joystick push button for user input.
 * - Clears LA Channel 2 initially.
 * - In an infinite loop:
 *   1. Sets LA Channel 2, capturing a start event.
 *   2. Toggles the red LED as a visual representation of task activity.
 *   3. Checks if the joystick button semaphore is available.
 *   4. If the semaphore is taken (button pressed):
 *      a. If Task 2 is not running, it creates Task 2.
 *      b. If Task 2 is already running, it deletes Task 2.
 *   5. Clears LA Channel 2, ending the event capture.
 * - Thus, this task provides user interactivity to control Task 2's execution using the joystick button.
 */
void Task3_Task( void ) {
	
	s4451119_reg_joystick_pb_init();
    BaseType_t xHigherPriorityTaskWoken;
	S4451119_REG_SYSMON_CHAN2_CLR();
	
    for (;;) {

		
		S4451119_REG_SYSMON_CHAN2_SET(); 
		BRD_LEDRedToggle();

		if (s4451119SemaphorePb != NULL) {
			if (xSemaphoreTake(s4451119SemaphorePb, 10) == pdTRUE) {
				if (task2Handle == NULL) {
					// Task 2 is not running, create it
					xTaskCreate((TaskFunction_t) Task2_Task, "Task 2", TASK2_STACK_SIZE, NULL, TASK2_PRIORITY, &task2Handle);
				} else {
					// Task 2 is running, delete it
					vTaskDelete(task2Handle);
					task2Handle = NULL;
				}
			}
   		}		
		S4451119_REG_SYSMON_CHAN2_CLR();
	}
	
}


/*
 * consoleTask: Visual Display Management with '>>' Movement
 * - Waits for a signal from Task 2 via a queue.
 * - When a signal is received:
 *   1. Clears the previous position of the '>>' marker in the display buffer.
 *   2. Moves the '>>' marker to a new position in the buffer.
 *   3. Prints the updated buffer to the console.
 * - This task gives a moving visual indication ('>>') on the console every time Task 2 completes its loop, thus providing a secondary sign of Task 2's execution.
 */
void consoleTask(void *pvParameters) {
    int message;
    int position = 0;  // To track the position of '>>'
    char consoleString[80] = {0}; // Buffer for the display string, initialized to all zeros

    while (1) {
		S4451119_REG_SYSMON_CHAN3_SET(); 
        // Wait indefinitely for a signal from Task2
        if (xQueueReceive(xQueue, &message, portMAX_DELAY)) {
            
            // Clear previous '>>' from the string
            consoleString[position] = ' ';
            consoleString[position + 1] = ' ';
            
            // Update position for '>>'
            position = (position + 2) % 30; 
            
            // Insert '>>' at the new position
            consoleString[position] = '>';
            consoleString[position + 1] = '>';
            
            // Display the updated string
            debug_log("%s\r", consoleString);
        }
    }
}


/*
 * Pushbutton callback function
 */
void s4451119_semaphore_pb()
{
	BaseType_t xHigherPriorityTaskWoken;

	if((GPIOA->IDR & (0x0001 << 3)) == (0x0001 << 3)) {
		// Is it time for another Task() to run?
		xHigherPriorityTaskWoken = pdFALSE;

		if (s4451119SemaphorePb != NULL) {	// Check if semaphore exists 
			xSemaphoreGiveFromISR( s4451119SemaphorePb, &xHigherPriorityTaskWoken );		// Give PB Semaphore from ISR
		}
		// Perform context switching, if required.
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken);

	}
}


/*
 * Interrupt handler for EXTI 15 to 10 IRQ Handler
 */ 
void EXTI3_IRQHandler(void) {

	NVIC_ClearPendingIRQ(EXTI3_IRQn);

	// PR: Pending register
	if ((EXTI->PR & EXTI_PR_PR3) == EXTI_PR_PR3) {

		// cleared by writing a 1 to this bit
		EXTI->PR |= EXTI_PR_PR3;	//Clear interrupt flag.

		s4451119_reg_joystick_pb_isr();
		s4451119_semaphore_pb();   // Callback for C13
		
		
	}
}