 /** 
 **************************************************************
 * @file mylib/s4451110_lta1000g.c
 * @author Sejin Son - 44511199
 * @date 27022023
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4451119_reg_lta1000g_init() - Initialise LED Bar GPIO pins as outputs.
 * lta1000g_seg_set(int segment, unsigned char countValue) - Set the value (0 or 1) for the corresponding LED Bar segment GPIO pin. (Internal function) 
 * s4451119_reg_lta1000g_write(unsigned short value) - Write the LED Bar segments high or low
 * s4451119_reg_lta1000g_invert(uint16_t value) - Invert bits correspoinding value
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "s4451119_joystick.h"


/* 	
 * @Global variables
 * @param ledOn = Giving a state of GPIO LED ON
 * @param ledOff = Giving a state of GPIO LED OFF
 */
unsigned char ledOn = 1;
unsigned char ledOff = 0;

/*
 * Initialise all GPIO PIN as output by setting bit for all. 
 */
extern void s4451119_reg_lta1000g_init() {

    /* Enable the GPIO Clocks */
    __GPIOC_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();


	/* Initialise C6 as an output. */
	GPIOC->MODER &= ~(0x03 << (6 * 2));  
	GPIOC->MODER |= (0x01 << (6 * 2));   
	GPIOC->OSPEEDR &= ~(0x03<<(6 * 2));
	GPIOC->OSPEEDR |=   0x02<<(6 * 2);  
	GPIOC->OTYPER &= ~(0x01 << 6);       

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOC->PUPDR &= ~(0x03 << (6 * 2));   
	GPIOC->PUPDR |= ((0x01) << (6 * 2));  
	GPIOC->ODR |= (0x01 << 6);   
	

	/* Initialise B15 as an output. */
	GPIOB->MODER &= ~(0x03 << (15 * 2));  
	GPIOB->MODER |= (0x01 << (15 * 2));   
	GPIOB->OSPEEDR &= ~(0x03<<(15 * 2));
	GPIOB->OSPEEDR |=   0x02<<(15 * 2);  
	GPIOB->OTYPER &= ~(0x01 << 15);       

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOB->PUPDR &= ~(0x03 << (15 * 2));   
	GPIOB->PUPDR |= ((0x01) << (15 * 2));  
	GPIOB->ODR |= (0x01 << 15);   

	/* Initialise B13 as an output. */
	GPIOB->MODER &= ~(0x03 << (13 * 2));  
	GPIOB->MODER |= (0x01 << (13 * 2));   
	GPIOB->OSPEEDR &= ~(0x03<<(13 * 2));
	GPIOB->OSPEEDR |=   0x02<<(13 * 2);  
	GPIOB->OTYPER &= ~(0x01 << 13);      

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOB->PUPDR &= ~(0x03 << (13 * 2));   
	GPIOB->PUPDR |= ((0x01) << (13 * 2));  
	GPIOB->ODR |= (0x01 << 13);   

	/* Initialise B12 as an output. */
	GPIOB->MODER &= ~(0x03 << (12 * 2));  
	GPIOB->MODER |= (0x01 << (12 * 2));   
	GPIOB->OSPEEDR &= ~(0x03<<(12 * 2));
	GPIOB->OSPEEDR |=   0x02<<(12 * 2);  
	GPIOB->OTYPER &= ~(0x01 << 12);       

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOB->PUPDR &= ~(0x03 << (12 * 2));   
	GPIOB->PUPDR |= ((0x01) << (12 * 2));  
	GPIOB->ODR |= (0x01 << 12);   
	
	/* Initialise A15 as an output. */
	GPIOA->MODER &= ~(0x03 << (15 * 2));  
	GPIOA->MODER |= (0x01 << (15 * 2));   
	GPIOA->OSPEEDR &= ~(0x03<<(15 * 2));
	GPIOA->OSPEEDR |=   0x02<<(15 * 2);  
	GPIOA->OTYPER &= ~(0x01 << 15);       

	/*  Activate the Pull-up or Pull down resistor for the current IO */
	GPIOA->PUPDR &= ~(0x03 << (15 * 2));  
	GPIOA->PUPDR |= ((0x01) << (15 * 2));  
	GPIOA->ODR |= (0x01 << 15);   

	/* Initialise C7 as an output. */
	GPIOC->MODER &= ~(0x03 << (7 * 2));  
	GPIOC->MODER |= (0x01 << (7 * 2));   
	GPIOC->OSPEEDR &= ~(0x03<<(7 * 2));
	GPIOC->OSPEEDR |=   0x02<<(7 * 2);  
	GPIOC->OTYPER &= ~(0x01 << 7);  

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOC->PUPDR &= ~(0x03 << (7 * 2));   
	GPIOC->PUPDR |= ((0x01) << (7 * 2));  
	GPIOC->ODR &= (0x01 << 7);   

	/* Initialise B5 as an output. */
	GPIOB->MODER &= ~(0x03 << (5 * 2));  
	GPIOB->MODER |= (0x01 << (5 * 2));   
	GPIOB->OSPEEDR &= ~(0x03<<(5 * 2));
	GPIOB->OSPEEDR |=   0x02<<(5 * 2);  
	GPIOB->OTYPER &= ~(0x01 << 5);       

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOB->PUPDR &= ~(0x03 << (5 * 2));   
	GPIOB->PUPDR |= ((0x01) << (5 * 2));  
	GPIOB->ODR &= (0x01 << 5);   

	/* Initialise B3 as an output. */
	GPIOB->MODER &= ~(0x03 << (3 * 2));  
	GPIOB->MODER |= (0x01 << (3 * 2));   
	GPIOB->OSPEEDR &= ~(0x03<<(3 * 2));
	GPIOB->OSPEEDR |=   0x02<<(3 * 2);  
	GPIOB->OTYPER &= ~(0x01 << 3);       

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOB->PUPDR &= ~(0x03 << (3 * 2));   
	GPIOB->PUPDR |= ((0x01) << (3 * 2));  
	GPIOB->ODR &= (0x01 << 3);   

	/* Initialise A4 as an output. */
	GPIOA->MODER &= ~(0x03 << (4 * 2));  
	GPIOA->MODER |= (0x01 << (4 * 2));   
	GPIOA->OSPEEDR &= ~(0x03<<(4 * 2));
	GPIOA->OSPEEDR |=   0x02<<(4 * 2);  
	GPIOA->OTYPER &= ~(0x01 << 4);      

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOA->PUPDR &= ~(0x03 << (4 * 2));   
	GPIOA->PUPDR |= ((0x01) << (4 * 2));  
	GPIOA->ODR &= (0x01 << 4);   

	/* Initialise B4 as an output. */
	GPIOB->MODER &= ~(0x03 << (4 * 2));  
	GPIOB->MODER |= (0x01 << (4 * 2));   
	GPIOB->OSPEEDR &= ~(0x03<<(4 * 2));
	GPIOB->OSPEEDR |=   0x02<<(4 * 2);  
	GPIOB->OTYPER &= ~(0x01 << 4);      

	/*  Activate the Pull-up or Pull down resistor for the current IO */
	GPIOB->PUPDR &= ~(0x03 << (4 * 2));   
	GPIOB->PUPDR |= ((0x01) << (4 * 2));  
	GPIOB->ODR &= (0x01 << 4);   
	
}

/*
 * By bit shifting, coparing bit(bitMask) with joystickPressCounter  
 * @param1 int segment : target segment of led.
 * @param2 int CountVaue : number of presses by pushbutton.
 * return : return 0.
 */
/*
 * Set the countcountValue (0 or 1) for the corresponding LED Bar segment GPIO pin. 
 */
void lta1000g_seg_set(int segment, unsigned char countValue) {
	/*
	 * Switch cases to set corresponding values to the registers of LED to turn on/off
	 */
	 switch (segment) {

		/* LED 1 (C6) */
        case 1:

            if (countValue == 1) {

                GPIOC->ODR |= (0x01 << 6);	

            } else {

                GPIOC->ODR &= ~(0x01 << 6);

            }
            break;

		/* LED 2 (B15) */
        case 2:
            if (countValue == 1) {

                GPIOB->ODR |= (0x01 << 15);

            } else {

                GPIOB->ODR &= ~(0x01 << 15);

			}
            break;

		/* LED 3 (B13) */
        case 3:

            if (countValue == 1) {

                GPIOB->ODR |= (0x01 << 13);

            } else {

                GPIOB->ODR &= ~(0x01 << 13);

            }
            break;

		/* LED 4 (B12) */	
        case 4:

            if (countValue == 1) {

                GPIOB->ODR |= (0x01 << 12);

            } else {

                GPIOB->ODR &= ~(0x01 << 12);

            }
            break;

		/* LED 5 (A15) */
        case 5:

            if (countValue == 1) {

                GPIOA->ODR |= (0x01 << 15);

            } else {

                GPIOA->ODR &= ~(0x01 << 15);

            }
            break;

		/* LED 6 (C7) */	
        case 6:

            if (countValue == 1) {

                GPIOC->ODR |= (0x01 << 7);

            } else {

                GPIOC->ODR &= ~(0x01 << 7);

            }
            break;

		/* LED 7 (B5) */
        case 7:

            if (countValue == 1) {

                GPIOB->ODR |= (0x01 << 5);

            } else {

                GPIOB->ODR &= ~(0x01 << 5);

            }
            break;

		/* LED 8 (B3) */	
        case 8:

            if (countValue == 1) {

                GPIOB->ODR |= (0x01 << 3);

            } else {

                GPIOB->ODR &= ~(0x01 << 3);

			}
			break;

		/* LED 9 (A4) */	
		case 9:

		    if (countValue == 1) {

                GPIOA->ODR |= (0x01 << 4);

            } else {

                GPIOA->ODR &= ~(0x01 << 4);

			}
			break;

		/* LED 10 (B4) */	
		case 10:

		    if (countValue == 1) {

                GPIOB->ODR |= (0x01 << 4);

            } else {

                GPIOB->ODR &= ~(0x01 << 4);

			}
			break;
	 }	

}

/*
 * Get the countValue variable and compare with bit shifted value and call seg_set function to make led on/off
 * Using bit iteration to compare with countValue and shifted bit.
 * @param input : countValue : number of presses by pushbutton
 * 
 */

extern void s4451119_reg_lta1000g_write(unsigned short countValue) {

    for (int i = 6; i < 11; i++) {

        if (countValue & (0x0001 << i)) {

            lta1000g_seg_set(i, ledOn);

        } else {

            lta1000g_seg_set(i, ledOff);

        }
    }
}

/*
 * Write the LED Bar segments high or low, depending on the value and mask. 
 * If a mask bit is set to 1, then the corresponding value bit must be inverted used and
 * then used to set LED Bar segment. 
 */
void s4451119_reg_lta1000g_invert(uint16_t value) {

	for (int i = 1; i < 11; i++) {

        if (i < 6) {

            // Invert the value bit for the first 5 segments
            if (value & (0x0001 << i)) {

                lta1000g_seg_set(i, ledOff);

            } else {

                lta1000g_seg_set(i, ledOn);

            }
        } else {
            // Do not invert the value bit for the remaining segments
			s4451119_reg_lta1000g_write(value);

		}
	}
}

