/**
 ******************************************************************************
 * @file    repo/s2/main.c
 * @author  Sejin Son
 * @date    13032022
 * @brief   Joystick Mylib Register Driver
 ******************************************************************************
 *
 */
#include "board.h"
#include "processor_hal.h"
#include "debug_log.h"
#include "s4451119_joystick.h"
#include "s4451119_pantilt.h"




int main(void)
{
	unsigned int adc_valuex;
	unsigned int adc_valuey;
	unsigned int dutycycle = 3;

	HAL_Init();
	s4451119_reg_joystick_init();
	s4451119_reg_pantilt_init();

	// Main Processing Loop
	while (1)
	{
		PWM_DUTYCYCLE_CHANGE1(PWM_PERCENT2TICKS_DUTYCYCLE(dutycycle));	//Set duty cyle
		PWM_DUTYCYCLE_CHANGE2(PWM_PERCENT2TICKS_DUTYCYCLE(dutycycle));
		
		
		dutycycle = (dutycycle + 1)%13;	//Increment by 10% from 10% to 100%
		HAL_Delay(50);					//Delay for 5s


		HAL_ADC_Start(&AdcHandley); // Start ADC conversion
		HAL_ADC_Start(&AdcHandlex);
	
		// Wait for ADC conversion to finish
		while ((HAL_ADC_PollForConversion(&AdcHandley, 20) != HAL_OK));
		adc_valuex = S4451119_REG_JOYSTICK_X_READ();
		adc_valuey = S4451119_REG_JOYSTICK_Y_READ();

		
		}

		
	}
	return 0;
}