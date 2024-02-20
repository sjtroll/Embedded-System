 /** 
 **************************************************************
 * @file mylib/s4451119_sysmon.h
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


extern void s4451119_sysmon_init();

//System monitor pins
#define SYSMON_CHAN0_PIN		10
#define SYSMON_CHAN0_GPIO		GPIOD
#define SYSMON_CHAN0_GPIO_CLK()	__GPIOD_CLK_ENABLE()

#define SYSMON_CHAN1_PIN		7
#define SYSMON_CHAN1_GPIO		GPIOG
#define SYSMON_CHAN1_GPIO_CLK()	__GPIOG_CLK_ENABLE()

#define SYSMON_CHAN2_PIN		4
#define SYSMON_CHAN2_GPIO		GPIOG
#define SYSMON_CHAN2_GPIO_CLK()	__GPIOG_CLK_ENABLE()

#define SYSMON_CHAN3_PIN		8
#define SYSMON_CHAN3_GPIO		GPIOD
#define SYSMON_CHAN3_GPIO_CLK()	__GPIOD_CLK_ENABLE()

//System monitor macros
#define S4451119_REG_SYSMON_CHAN0_SET()		SYSMON_CHAN0_GPIO->ODR |= (0x01 << SYSMON_CHAN0_PIN);
#define S4451119_REG_SYSMON_CHAN0_CLR()		SYSMON_CHAN0_GPIO->ODR &= ~(0x01 << SYSMON_CHAN0_PIN);

#define S4451119_REG_SYSMON_CHAN1_SET()		SYSMON_CHAN1_GPIO->ODR |= (0x01 << SYSMON_CHAN1_PIN);
#define S4451119_REG_SYSMON_CHAN1_CLR()		SYSMON_CHAN1_GPIO->ODR &= ~(0x01 << SYSMON_CHAN1_PIN);

#define S4451119_REG_SYSMON_CHAN2_SET()		SYSMON_CHAN2_GPIO->ODR |= (0x01 << SYSMON_CHAN2_PIN);
#define S4451119_REG_SYSMON_CHAN2_CLR()		SYSMON_CHAN2_GPIO->ODR &= ~(0x01 << SYSMON_CHAN2_PIN);

#define S4451119_REG_SYSMON_CHAN3_SET()		SYSMON_CHAN3_GPIO->ODR |= (0x01 << SYSMON_CHAN3_PIN);
#define S4451119_REG_SYSMON_CHAN3_CLR()		SYSMON_CHAN3_GPIO->ODR &= ~(0x01 << SYSMON_CHAN3_PIN);
