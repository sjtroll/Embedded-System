/** 
 **************************************************************
 * @file mylib/s4451119_joystick.h
 * @author Sejin Son - 44511199
 * @date 02082023
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


extern void s4451119_reg_joystick_pb_init();
extern void s4451119_reg_joystick_init();
void s4451119_reg_joystick_press_reset();
extern int s4451119_reg_joystick_press_get();
extern void s4451119_reg_joystick_pb_isr();
static int previousTick;
static int joystickPressCounter;
int s4451119_joystick_getx(ADC_HandleTypeDef *AdcHandle);
#define S4451119_REG_JOYSTICK_X_READ() s4451119_joystick_getx(&AdcHandlex);
#define S4451119_REG_JOYSTICK_X_ZERO_CAL_OFFSET 35	
#define ADC_MAX_VALUE 4095
ADC_HandleTypeDef AdcHandlex;

