 /** 
 **************************************************************
 * @file mylib/s4451119_joystick.c
 * @author Sejin Son - 44511199
 * @date 14082023
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4451119_reg_joystick_pb_init() - Enabled the Joystick pushbutton source
 * s4451119_reg_joystick_press_get() - Returns the value of the Joystick pushbutton press counter 
 * s4451119_reg_joystick_press_reset() - Reset the Joystick event counter value to 0
 * s4451119_reg_joystick_pb_isr() - Joystick Pushbutton Interrupt service routine. Must be called from the corresponding GPIO EXTI interrupt service routine
 *************************************************************** 
 */



#include "board.h"
#include "processor_hal.h"
#include "debug_log.h"
#include "s4451119_lta1000g.h"
#include "s4451119_rgb.h"
#include "s4451119_hamming.h"



ADC_HandleTypeDef AdcHandlex;
ADC_ChannelConfTypeDef AdcChanConfigx;

/*
 * @Global variable 
 * @param joystickPressCounter = number of joystick press.
 * @param previousTime = To adjust system tick.
 */
static int joystickPressCounter = 0;
static int previousTime = 0;



/*
 * General Init function
 * Initialise GPIO pins and ADC on PC0 for z-axis
 */
extern void s4451119_reg_joystick_init() {

	// Enable the GPIO Clock
  	__GPIOC_CLK_ENABLE();
  
	// Initalise PC0 as an Analog input.
  	GPIOC->MODER |= (0x03 << (0 * 2));			//Set bits for Analog input mode
  	GPIOC->OSPEEDR &= ~(0x03<<(0 * 2));
	GPIOC->OSPEEDR |= 0x02<<(0 * 2);  			// Fast speed
	GPIOC->PUPDR &= ~(0x03 << (0 * 2));			//Clear bits for no push/pull

	__ADC1_CLK_ENABLE();

	AdcHandlex.Instance = (ADC_TypeDef *)(ADC1_BASE);						//Use ADC1 (For PC0)
	AdcHandlex.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;	//Set clock prescaler
	AdcHandlex.Init.Resolution            = ADC_RESOLUTION12b;				//Set 12-bit data resolution
	AdcHandlex.Init.ScanConvMode          = DISABLE;
	AdcHandlex.Init.ContinuousConvMode    = DISABLE;
	AdcHandlex.Init.DiscontinuousConvMode = DISABLE;
	AdcHandlex.Init.NbrOfDiscConversion   = 0;
	AdcHandlex.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;	//No Trigger
	AdcHandlex.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;		//No Trigger
	AdcHandlex.Init.DataAlign             = ADC_DATAALIGN_RIGHT;				//Right align data
	AdcHandlex.Init.NbrOfConversion       = 1;
	AdcHandlex.Init.DMAContinuousRequests = DISABLE;
	AdcHandlex.Init.EOCSelection          = DISABLE;

	// Configure ADC Channel
	AdcChanConfigx.Channel = ADC_CHANNEL_10;					//PC0 has Analog Channel 3 connected
	AdcChanConfigx.Rank         = 1;
	AdcChanConfigx.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	AdcChanConfigx.Offset       = 0;

	HAL_ADC_Init(&AdcHandlex);		//Initialise ADC
	HAL_ADC_ConfigChannel(&AdcHandlex, &AdcChanConfigx);		//Initialise ADC Channe


}

/*
 * Generic function to retrieve the ADC value corresponding to the X-axis of the joystick.
 * 
 * This function accesses the Direct Register (`DR`) of the specified ADC instance 
 * (provided via the AdcHandle) to obtain the latest ADC conversion result. The Direct Register
 * contains the result of the conversion and is immediately readable as soon as the conversion is 
 * complete.
 * 
 * @param input - AdcHandle: Pointer to the ADC_HandleTypeDef structure that contains the 
 *                configuration information for the specified ADC.
 * 
 * @return: The ADC value corresponding to the X-axis of the joystick.
 */
extern int s4451119_joystick_getx(ADC_HandleTypeDef *AdcHandle) {

    return AdcHandle->Instance->DR;

}



/*
 * Initializes the Joystick pushbutton source by enabling the necessary hardware features:
 * GPIOC Clock, setting GPIOA speed and mode, enabling EXTI clock, setting the trigger source,
 * enabling the rising edge and external interrupt, and setting the interrupt priority and callback.
 */
extern void s4451119_reg_joystick_pb_init(){

    /* Enable GPIOC Clock */
	__GPIOA_CLK_ENABLE();

    GPIOA->OSPEEDR |= (GPIO_SPEED_FAST << 3);	
	GPIOA->PUPDR &= ~(0x03 << (3 * 2));			
	GPIOA->MODER &= ~(0x03 << (3 * 2));			

	/* Enable EXTI clock */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* select trigger source (port a, pin 3) on EXTICR3. */
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA;

    /* enable rising edge */
	EXTI->RTSR |= EXTI_RTSR_TR3;
    /* disable falling edge */	
	EXTI->FTSR &= ~EXTI_FTSR_TR3;	
    /*enable external interrupt */
	EXTI->IMR |= EXTI_IMR_IM3;		

	/* Enable priority (10) and interrupt callback. Do not set a priority lower than 5. */
	HAL_NVIC_SetPriority(EXTI3_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

}


/*
 * Interrupt service routine (ISR). This function is called from the GPIO EXTI 
 * interrupt service routine. The external interrupt implementation uses the system 
 * time to debounce the joystick push button press event.
 *
 * If the push button has been pressed and more than 1000ms (1 second) has passed 
 * since the last press, the joystickPressCounter is incremented. If the counter
 * reaches 5, it is reset to 0. The function also sets or clears the GPIOA Output 
 * Data Register (ODR) depending on the button press state.
 *
 * @variable currentTime : Current system time obtained from HAL_GetTick().
 * @variable joystickPressCounter : Counts the number of joystick button presses.
 *                                  Incremented upon each button press and reset
 *                                  to 0 when it reaches 5.
 */
extern void s4451119_reg_joystick_pb_isr() {
	int currentTime = HAL_GetTick();

    if (currentTime - previousTime > 1000) {	
		if ((GPIOA->IDR & (0x0001 << 3)) == (0x0001 << 3)) {
			GPIOA->ODR &= ~(0x01 << 3);	
		} else {
			GPIOA->ODR |= (0x01 << 3);	
		}
    }
	previousTime = currentTime;
}

