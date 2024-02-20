/**
 ******************************************************************************
 * @file    repo/s3/main.c
 * @author  Sejin Son
 * @date    28082023
 * @brief   Hamming Encoder/Decoder with Console input (Stage 3)
 ******************************************************************************
 *
 */


#include "board.h"
#include "processor_hal.h"
#include "debug_log.h"
#include "s4451119_hamming.h"
#include "s4451119_lta1000g.h"
#include "s4451119_console.h"
#include "s4451119_joystick.h"

/* Define FSM states */
#define S0  0  // IDLE State
#define S1  1  // Encoding State
#define S2  2  // Decoding State



void main(void) {

	int displayFlag = 1;
    int state = S0;
    unsigned char recvChar;
	unsigned char secondChar;
	unsigned int inputCount = 0;
    int prevTick = 0;

	HAL_Init();	
	BRD_LEDInit();		//Initialise Blue LED
	BRD_debuguart_init();  //Initialise UART for debug log output
	s4451119_reg_lta1000g_init();
	s4451119_reg_joystick_pb_init();


	/* Main processing loop */
    while (1) {
        if ((HAL_GetTick() - prevTick) >= 2000) {
            prevTick =  HAL_GetTick();
        }
        // Check if a character is received on UART
        if ((recvChar = BRD_debuguart_getc()) != '\0') {
            // Process received character through the FSM to determine the next state
			state = s4451119_lib_console_ascii2hex(state, secondChar, recvChar, inputCount);
			inputCount++;
            // Reset inputCount after 2 characters are processed
			if (inputCount > 1) {
				inputCount = 0;
			}
			secondChar = recvChar;  // Store the current received character as the last received character for the next iteration
        }
  	}
}
