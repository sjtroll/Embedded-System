/** 
 **************************************************************
 * @file mylib/s4451119_reg_rgb.h
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



#define TIMER_COUNTER_FREQ  				10000          				//Frequency (in Hz)
#define PWM_PULSE_WIDTH_SECONDS				0.005					//5ms
#define PWM_PULSE_WIDTH_TICKS				TIMER_COUNTER_FREQ * PWM_PULSE_WIDTH_SECONDS					//((PWM_PULSE_WIDTH_SECONDS)/(1/TIMER_COUNTER_FREQ))		//Period in timer ticks
#define PWM_PERCENT2TICKS_DUTYCYCLE(value)	(uint16_t)(value*PWM_PULSE_WIDTH_TICKS/100)				//Duty Cycle on time in timer ticks
#define PWM_DUTYCYCLE_CHANGE1(value) 		TIM1->CCR1=(uint16_t)value
int brightness;
extern void s4451119_reg_rgb_init(); 
extern void s4451119_reg_rgb_brightness_write(int level);
extern void s4451119_reg_rgb_colour_set(int angle);
extern int s4451119_reg_rgb_brightness_isr();
extern void s4451119_reg_rgb_brightness_read();
static int previousTime;
