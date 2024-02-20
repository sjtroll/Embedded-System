/** 
 **************************************************************
 * @file mylib/s4451119_sysmon.c
 * @author Sejin Son - 44511199
 * @date 10/09/23
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4451119_sysmon_init( void ) - Hardware Initialisation - MUST be called from a task and not in main.
 *************************************************************** 
 */


#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* System monitor pins */
#define SYSMON_CHAN0_PIN		10
#define SYSMON_CHAN0_GPIO		GPIOD
#define SYSMON_CHAN0_GPIO_CLK()	__GPIOD_CLK_ENABLE()

#define SYSMON_CHAN1_PIN		7
#define SYSMON_CHAN1_GPIO		GPIOG
#define SYSMON_CHAN1_GPIO_CLK()	__GPIOG_CLK_ENABLE()

#define SYSMON_CHAN2_PIN		4
#define SYSMON_CHAN2_GPIO		GPIOG
#define SYSMON_CHAN2_GPIO_CLK()	__GPIOG_CLK_ENABLE()



/* System monitor macros */
#define S4451119_REG_SYSMON_CHAN0_SET()		SYSMON_CHAN0_GPIO->ODR |= (0x01 << SYSMON_CHAN0_PIN);
#define S4451119_REG_SYSMON_CHAN0_CLR()		SYSMON_CHAN0_GPIO->ODR &= ~(0x01 << SYSMON_CHAN0_PIN);

#define S4451119_REG_SYSMON_CHAN1_SET()		SYSMON_CHAN1_GPIO->ODR |= (0x01 << SYSMON_CHAN1_PIN);
#define S4451119_REG_SYSMON_CHAN1_CLR()		SYSMON_CHAN1_GPIO->ODR &= ~(0x01 << SYSMON_CHAN1_PIN);

#define S4451119_REG_SYSMON_CHAN2_SET()		SYSMON_CHAN2_GPIO->ODR |= (0x01 << SYSMON_CHAN2_PIN);
#define S4451119_REG_SYSMON_CHAN2_CLR()		SYSMON_CHAN2_GPIO->ODR &= ~(0x01 << SYSMON_CHAN2_PIN);




/*
 * Hardware Initialisation - MUST be alled from a task and not in main.
 */
extern void s4451119_sysmon_init( void ) {

	taskENTER_CRITICAL();	//Stop any interruption of the critical section

	BRD_LEDInit();				//Initialise Green LED
	BRD_LEDGreenOff();			//Turn off Green LED
	BRD_LEDBlueOff();			//Turn off Green LED
	BRD_LEDRedOff();			//Turn off Green LED

	// Enable the GPIO G Clock
  	SYSMON_CHAN0_GPIO_CLK();

  	//Initialise G9 as an output.
  	SYSMON_CHAN0_GPIO->MODER &= ~(0x03 << (SYSMON_CHAN0_PIN * 2));  //clear bits
  	SYSMON_CHAN0_GPIO->MODER |= (0x01 << (SYSMON_CHAN0_PIN * 2));   //Set for push pull

  	SYSMON_CHAN0_GPIO->OSPEEDR &= ~(0x03<<(SYSMON_CHAN0_PIN * 2));
	SYSMON_CHAN0_GPIO->OSPEEDR |=   0x02<<(SYSMON_CHAN0_PIN * 2);  // Set for Fast speed

  	SYSMON_CHAN0_GPIO->OTYPER &= ~(0x01 << SYSMON_CHAN0_PIN);       //Clear Bit for Push/Pull utput

  	// Activate the Pull-up or Pull down resistor for the current IO
  	SYSMON_CHAN0_GPIO->PUPDR &= ~(0x03 << (SYSMON_CHAN0_PIN * 2));   //Clear Bits
  	SYSMON_CHAN0_GPIO->PUPDR |= ((0x01) << (SYSMON_CHAN0_PIN * 2));  //Set for Pull down output
  
	SYSMON_CHAN1_GPIO_CLK();

  	//Initialise G9 as an output.
  	SYSMON_CHAN1_GPIO->MODER &= ~(0x03 << (SYSMON_CHAN1_PIN * 2));  //clear bits
  	SYSMON_CHAN1_GPIO->MODER |= (0x01 << (SYSMON_CHAN1_PIN * 2));   //Set for push pull

  	SYSMON_CHAN1_GPIO->OSPEEDR &= ~(0x03<<(SYSMON_CHAN1_PIN * 2));
	SYSMON_CHAN1_GPIO->OSPEEDR |=   0x02<<(SYSMON_CHAN1_PIN * 2);  // Set for Fast speed

  	SYSMON_CHAN1_GPIO->OTYPER &= ~(0x01 << SYSMON_CHAN1_PIN);       //Clear Bit for Push/Pull utput

  	// Activate the Pull-up or Pull down resistor for the current IO
  	SYSMON_CHAN1_GPIO->PUPDR &= ~(0x03 << (SYSMON_CHAN1_PIN * 2));   //Clear Bits
  	SYSMON_CHAN1_GPIO->PUPDR |= ((0x01) << (SYSMON_CHAN1_PIN * 2));  //Set for Pull down output

	SYSMON_CHAN2_GPIO_CLK();

  	//Initialise G9 as an output.
  	SYSMON_CHAN2_GPIO->MODER &= ~(0x03 << (SYSMON_CHAN2_PIN * 2));  //clear bits
  	SYSMON_CHAN2_GPIO->MODER |= (0x01 << (SYSMON_CHAN2_PIN * 2));   //Set for push pull

  	SYSMON_CHAN2_GPIO->OSPEEDR &= ~(0x03<<(SYSMON_CHAN2_PIN * 2));
	SYSMON_CHAN2_GPIO->OSPEEDR |=   0x02<<(SYSMON_CHAN2_PIN * 2);  // Set for Fast speed

  	SYSMON_CHAN2_GPIO->OTYPER &= ~(0x01 << SYSMON_CHAN2_PIN);       //Clear Bit for Push/Pull utput

  	// Activate the Pull-up or Pull down resistor for the current IO
  	SYSMON_CHAN2_GPIO->PUPDR &= ~(0x03 << (SYSMON_CHAN2_PIN * 2));   //Clear Bits
  	SYSMON_CHAN2_GPIO->PUPDR |= ((0x01) << (SYSMON_CHAN2_PIN * 2));  //Set for Pull down output


	taskEXIT_CRITICAL();
}