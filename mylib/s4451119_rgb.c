 /** 
 **************************************************************
 * @file mylib/s4451119_rgb.c
 * @author Sejin Son - 44511199
 * @date 14082023
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4451119_reg_rgb_init() - initialise RGB LED (GPIO, PWM, etc)
 * s4451119_reg_rgb_brightness_write(int level) - Get the level variable and compare with bit shifted value and call read() function to make led on/off
 * s4451119_reg_rgb_colour__get_set - Set the primary colour of the RGB Led. 
 * s4451119_reg_rgb_colour__get_get (unsigned char rgb_mask) - Return current primary colour of the RGB Led as an RGB bit mask
 *************************************************************** 
 */


#define TIMER_COUNTER_FREQ  				10000   //Frequency (in Hz)
#define PWM_PULSE_WIDTH_SECONDS				0.005	// 0.005s
#define PWM_PULSE_WIDTH_TICKS				TIMER_COUNTER_FREQ * PWM_PULSE_WIDTH_SECONDS //((PWM_PULSE_WIDTH_SECONDS)/(1/TIMER_COUNTER_FREQ))		//Period in timer ticks
#define PWM_PERCENT2TICKS_DUTYCYCLE(value)	(uint16_t)(value*PWM_PULSE_WIDTH_TICKS/100)	//Duty Cycle on time in timer ticks
#define PWM_DUTYCYCLE_CHANGE1(value) 		TIM1->CCR1=(uint16_t)value
#define S4451119_REG_RGB_Black  0x00
#define S4451119_REG_RGB_Blue  0x01
#define S4451119_REG_RGB_Green  0x02
#define S4451119_REG_RGB_Cyan  0x03
#define S4451119_REG_RGB_Red  0x04
#define S4451119_REG_RGB_Magenta  0x05
#define S4451119_REG_RGB_Yellow  0x06
#define S4451119_REG_RGB_White  0x07

#include "board.h"
#include "processor_hal.h"
#include "s4451119_lta1000g.h"
#include "s4451119_joystick.h"
#include "debug_log.h"


/* 	
 * @Global variables
 * @param turnOn = Giving a state of GPIO LED ON
 * @param turnOff = Giving a state of GPIO LED OFF
 * @param brightness = Variable to store the current brightness level of the LED, ranging from 0-100.
 */
unsigned char turnOn = 1;
unsigned char turnOff = 0;
static int previousTime = 0;
int brightness = 0;


/*
 * General Init function
 * Initialise PWM output on PE9 pin and 3 GPIO output on PE11, PF14, PE13 pins
 * Called from main function
 */
extern void s4451119_reg_rgb_init() {

	// Enable GPIOC Clock
	__GPIOE_CLK_ENABLE();
	__GPIOF_CLK_ENABLE();

	/* Initialise PE9 as an PWM output */
	GPIOE->OSPEEDR |= (GPIO_SPEED_FAST << 2);		
	GPIOE->PUPDR &= ~(0x03 << (9 * 2));				
	GPIOE->MODER &= ~(0x03 << (9 * 2));				
	GPIOE->MODER |= (GPIO_MODE_AF_PP << (9 * 2));  	
	GPIOE->AFR[1] &= ~((0x0F) << (1 * 4));			
	GPIOE->AFR[1] |= (GPIO_AF1_TIM1 << (1 * 4));	

	/* Initialise PE11 as an output. (RED) */
	GPIOE->MODER &= ~(0x03 << (11 * 2));  
	GPIOE->MODER |= (0x01 << (11 * 2));   
	GPIOE->OSPEEDR &= ~(0x03<<(11 * 2));
	GPIOE->OSPEEDR |=   0x02<<(11 * 2);  
	GPIOE->OTYPER &= ~(0x01 << 11);      
	/*  Activate the Pull-up or Pull down resistor for the current IO */
	GPIOE->PUPDR &= ~(0x03 << (11 * 2));   
	GPIOE->PUPDR |= ((0x01) << (11 * 2));  
	GPIOE->ODR &= ~(0x01 << 11);   


	/* Initialise PF14 as an output. (GREED) */
	GPIOF->MODER &= ~(0x03 << (14 * 2));  
	GPIOF->MODER |= (0x01 << (14 * 2));   
	GPIOF->OSPEEDR &= ~(0x03<<(14 * 2));
	GPIOF->OSPEEDR |=   0x02<<(14 * 2);  
	GPIOF->OTYPER &= ~(0x01 << 14);      
	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOF->PUPDR &= ~(0x03 << (14 * 2));   
	GPIOF->PUPDR |= ((0x01) << (14 * 2));  
	GPIOF->ODR |= (0x01 << 14);   


	/* Initialise PE13 as an output. (BLUE)*/
	GPIOE->MODER &= ~(0x03 << (13 * 2));  
	GPIOE->MODER |= (0x01 << (13 * 2));   
	GPIOE->OSPEEDR &= ~(0x03<<(13 * 2));
	GPIOE->OSPEEDR |=   0x02<<(13 * 2);  
	GPIOE->OTYPER &= ~(0x01 << 13);      
	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOE->PUPDR &= ~(0x03 << (13 * 2));   
	GPIOE->PUPDR |= ((0x01) << (13 * 2));  
	GPIOE->ODR |= (0x01 << 13);   


	// Timer 1 clock enable
	__TIM1_CLK_ENABLE();

	/* Compute the prescaler value
	   Set the clock prescaler to 50kHz
	   SystemCoreClock is the system clock frequency */
	TIM1->PSC = ((SystemCoreClock / 2) / TIMER_COUNTER_FREQ) - 1;

	// Counting direction: 0 = up-counting, 1 = down-counting (Timer Control Register 1)
	TIM1->CR1 &= ~TIM_CR1_DIR; 

	TIM1->ARR = PWM_PULSE_WIDTH_TICKS; 		//Set pulse period to ~1s 
	TIM1->CCR1 = PWM_PERCENT2TICKS_DUTYCYCLE(100);

	TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M); 	// Clear OC1M (Channel 1) 
	TIM1->CCMR1 |= (0x6 << 4); 		// Enable PWM Mode 1, upcounting, on Channel 1 
	TIM1->CCMR1 |= (TIM_CCMR1_OC1PE); 	// Enable output preload bit for channel 1 
	
	TIM1->CR1  |= (TIM_CR1_ARPE); 	// Set Auto-Reload Preload Enable 
	TIM1->CCER |= TIM_CCER_CC1E; 	// Set CC1E Bit
	TIM1->CCER &= ~TIM_CCER_CC1NE; 	// Clear CC1NE Bit for active high output
	
	/* Set Main Output Enable (MOE) bit
	   Set Off-State Selection for Run mode (OSSR) bit
	   Set Off-State Selection for Idle mode (OSSI) bit */
	TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI; 

	TIM1->CR1 |= TIM_CR1_CEN;	// Enable the counter
}

/*
 * Applies the current brightness setting to the PWM's duty cycle.
 * 
 * The function retrieves the global brightness setting and then adjusts 
 * the PWM's duty cycle to match it. The transformation of brightness 
 * to PWM ticks is handled by the `PWM_PERCENT2TICKS_DUTYCYCLE` macro.
 * 
 * Note: It's presumed that the `brightness` variable is a global or external
 *       variable that contains the current brightness setting. This function 
 *       does not actually "read" the brightness in the traditional sense, but 
 *       rather it applies the brightness setting to the PWM.
 */
extern void s4451119_reg_rgb_brightness_read() {

    /* Adjust the PWM's duty cycle to match the current brightness setting. */
    PWM_DUTYCYCLE_CHANGE1(PWM_PERCENT2TICKS_DUTYCYCLE(brightness));
}


/*
 * Adjust the LED segment display to represent the brightness level based on an ADC input value.
 * 
 * The function breaks down the ADC input value (`level`) to determine the number of segments to activate.
 * The logic works as follows:
 *   - For a `level` value of 0 to 9, the first LED segment is activated.
 *   - For a `level` value of 10 to 19, the first two LED segments are activated.
 *   - ... and so on.
 * The remaining segments that don't fit within the current brightness level are turned off.
 * 
 * @param input -- level: ADC input value indicating the Y-axis movement. This is used to represent the 
 *                 desired brightness level. The ADC value is divided by 10 to map to the corresponding 
 *                 number of LED segments to light up.
 */
extern void s4451119_reg_rgb_brightness_write(int level) {

    /* Determines the number of segments to light up based on the current brightness level. */
    int num_led = (level / 10) + 1;

    /* Lights up the required number of segments to represent the brightness. */
    for (int i = 0; i < num_led; i++) {
        lta1000g_seg_set(i, turnOn);
    }

    /* Turns off the segments that are not needed for the current brightness level. */
    for (int i = num_led; i < 11; i++) {
        lta1000g_seg_set(i, turnOff);
    }
}


/*
 * Set the primary colour of the RGB LED using GPIO pins. The RGB bit mask is structured as < R >< G >< B >. 
 * For instance, a mask value of 0x04 would activate the red color component of the RGB LED.
 * 
 * Each color component of the LED (Red, Green, and Blue) is controlled by setting GPIO pins either high (1) or low (0).
 * This function serves as the low-level driver responsible for controlling these GPIO pins.
 *
 * Internal utility function primarily invoked from s4451119_reg_rgb_colour_get(int angle) function.
 *
 * @param input -- rgb_mask: The desired primary color represented by a bit mask.
 */
void s4451119_reg_rgb_colour__get(unsigned char rgb_mask) {

    switch (rgb_mask) {

        /* Set RGB LED to Black (All components OFF) */
        case S4451119_REG_RGB_Black:
            GPIOE->ODR &= ~(0x01 << 11);
            GPIOF->ODR &= ~(0x01 << 14);
            GPIOE->ODR &= ~(0x01 << 13);
            break;

        /* Set RGB LED to Blue (Only Blue component ON) */
        case S4451119_REG_RGB_Blue:
            GPIOE->ODR &= ~(0x01 << 11);
            GPIOF->ODR &= ~(0x01 << 14);
            GPIOE->ODR |= (0x01 << 13);
            break;

        /* Set RGB LED to Green (Only Green component ON) */
        case S4451119_REG_RGB_Green:
            GPIOE->ODR &= ~(0x01 << 11);
            GPIOF->ODR |= (0x01 << 14);
            GPIOE->ODR &= ~(0x01 << 13);
            break;

        /* Set RGB LED to Cyan (Green and Blue components ON) */
        case S4451119_REG_RGB_Cyan:
            GPIOE->ODR &= ~(0x01 << 11);
            GPIOF->ODR |= (0x01 << 14);
            GPIOE->ODR |= (0x01 << 13);
            break;

        /* Set RGB LED to Red (Only Red component ON) */
        case S4451119_REG_RGB_Red:
            GPIOE->ODR |= (0x01 << 11);
            GPIOF->ODR &= ~(0x01 << 14);
            GPIOE->ODR &= ~(0x01 << 13);
            break;

        /* Set RGB LED to Magenta (Red and Blue components ON) */
        case S4451119_REG_RGB_Magenta:
            GPIOE->ODR |= (0x01 << 11);
            GPIOF->ODR &= ~(0x01 << 14);
            GPIOE->ODR |= (0x01 << 13);
            break;

        /* Set RGB LED to Yellow (Red and Green components ON) */
        case S4451119_REG_RGB_Yellow:
            GPIOE->ODR |= (0x01 << 11);
            GPIOF->ODR |= (0x01 << 14);
            GPIOE->ODR &= ~(0x01 << 13);
            break;

        /* Set RGB LED to White (All components ON) */
        case S4451119_REG_RGB_White:
            GPIOE->ODR |= (0x01 << 11);
            GPIOF->ODR |= (0x01 << 14);
            GPIOE->ODR |= (0x01 << 13);
            break;
    }
}

/*
 * Set the primary colour of the RGB Led based on the joystick's X-axis angle.
 * The RGB bit mask is structured as < R >< G >< B >, where, for instance, a value of 0x04 would set the colour to red.
 * This function only manipulates GPIO pins to set them high or low based on the angle value.
 * The actual setting of GPIO pins is done by calling the s4451119_reg_rgb_colour_get() function with appropriate color constants.
 * 
 * @param input - angle: The converted ADC value representing the X-axis position of the joystick.
 */
void s4451119_reg_rgb_colour_set(int angle) {

    /* If the joystick angle is less than 40, set the RGB LED color to Black. */
    if (angle < 40) {
        s4451119_reg_rgb_colour__get(S4451119_REG_RGB_Black);
    }
    /* If the joystick angle is between 40 and 65, set the RGB LED color to Blue */
    else if (angle < 65) {
        s4451119_reg_rgb_colour__get(S4451119_REG_RGB_Blue);
    }
    /* If the joystick angle is between 65 and 85, set the RGB LED color to Green. */
    else if (angle < 85) {
        s4451119_reg_rgb_colour__get(S4451119_REG_RGB_Green);
    }
    /* If the joystick angle is between 85 and 95, set the RGB LED color to Cyan */
    else if (angle < 95) {
        s4451119_reg_rgb_colour__get(S4451119_REG_RGB_Cyan);
    }
    /* If the joystick angle is between 95 and 115, set the RGB LED color to Red. */
    else if (angle < 115) {
        s4451119_reg_rgb_colour__get(S4451119_REG_RGB_Red);
    }
    /* If the joystick angle is between 115 and 125, set the RGB LED color to Magenta. */
    else if (angle < 125) {
        s4451119_reg_rgb_colour__get(S4451119_REG_RGB_Magenta);
    }
    /* If the joystick angle is between 125 and 135, set the RGB LED color to Yellow. */
    else if (angle < 135) {
        s4451119_reg_rgb_colour__get(S4451119_REG_RGB_Yellow);
    }
    /* If the joystick angle is 145 or greater, set the RGB LED color to White. */
    else if (angle >= 145) {
        s4451119_reg_rgb_colour__get(S4451119_REG_RGB_White);
    }
}

/*
 * ISR for adjusting LED brightness upon a button press.
 * When the button is pressed, it increases the LED brightness by increments of 20%.  
 * If brightness exceeds 100%, it's reset to 0%.
 * 
 */ 
extern int s4451119_reg_rgb_brightness_isr() {

	int currentTime = HAL_GetTick();

    if (currentTime - previousTime > 1000) {    
        if ((GPIOA->IDR & (0x0001 << 3)) == (0x0001 << 3)) {

			/* Incrementing brightness by 20 */
				brightness += 20;
				debug_log("pushed!");
				if (brightness >= 101) {
					brightness = 0;
				}
            GPIOA->ODR &= ~(0x01 << 3);
		
        } else {
            GPIOA->ODR |= (0x01 << 3);
        }
    }    
    previousTime = currentTime;
	return brightness;
}