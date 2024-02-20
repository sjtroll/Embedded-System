/**
 ******************************************************************************
 * @file    repo/s2/main.c
 * @author  Sejin Son
 * @date    14082023
 * @brief   Joystick Mylib Register Driver
 ******************************************************************************
 *
 */
#include "board.h"
#include "processor_hal.h"
#include "debug_log.h"
#include "s4451119_joystick.h"
#include "s4451119_lta1000g.h"
#include "s4451119_rgb.h"

void EXTI3_IRQHandler(void);

int main(void)
{

	HAL_Init();
	BRD_debuguart_init();
	s4451119_reg_lta1000g_init();
	s4451119_reg_joystick_init();
	s4451119_reg_rgb_init();
	s4451119_reg_joystick_pb_init();


	// Main Processing Loop
	while (1)
	{		
		/* Start the ADC conversion process. */
		HAL_ADC_Start(&AdcHandlex);
		
		/* Wait for the ADC conversion to complete */
		while ((HAL_ADC_PollForConversion(&AdcHandlex, 10) != HAL_OK));
		
		/* Obtain the ADC value for the x-axis of the joystick */
		unsigned int adc_valuex = S4451119_REG_JOYSTICK_X_READ();
		
		/* AAdjust the x-axis ADC value to account for calibration and scaling factors, making it more intuitive to use.*/
		int colour = ((adc_valuex * 110)/ADC_MAX_VALUE) + S4451119_REG_JOYSTICK_X_ZERO_CAL_OFFSET;
		
		
		/* Print the current brightness and colour value to the debug UART for monitoring purposes. */
		debug_log("dutycycle : %d\r\n", brightness);
		debug_log("Colour : %d\r\n", colour);
		
		/* Retrieve and apply the current brightness value for the RGB LED. */
		s4451119_reg_rgb_brightness_read();
		s4451119_reg_rgb_brightness_write(brightness);
		
		/* Set the RGB LED's color based on the processed joystick x-axis value. */
		s4451119_reg_rgb_colour_set(colour);
		
	
	}
	
	return 0;
}