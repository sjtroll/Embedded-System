/**
 ******************************************************************************
 * @file    repo/s1/main.c
 * @author  Sejin Son
 * @date    02082023
 * @brief   GPIO and Pushbutton Debouncing (Stage 1)
 ******************************************************************************
 *
 */

#include "board.h"
#include "processor_hal.h"
#include "s4451119_joystick.h"
#include "s4451119_lta1000g.h"
#include "debug_log.h"


void EXTI3_IRQHandler(void);



int main(void) {
	
	unsigned short value = 0x01;  // Starting with the first LED on
    int speed = 1000;  // Cursor speed, initially 10 seconds
	
	

	HAL_Init();			//Initialise board.
	BRD_debuguart_init();

	/*Initialise LED Bar GPIO pins as outputs. */
	s4451119_reg_lta1000g_init();

	/* Enabled the Joystick pushbutton source, e.g. enables GPIO input and interrupt. */
	s4451119_reg_joystick_pb_init();

	/* Infinite loop */
	while (1) {

    int delayTime = speed / (1 << joystickPressCounter);
    value = s4451119_reg_lta1000g_lrotate(value, 1);
	
    HAL_Delay(delayTime);

    debug_log("press counter: %d\n\r", joystickPressCounter);
	}

	return 0;
}

