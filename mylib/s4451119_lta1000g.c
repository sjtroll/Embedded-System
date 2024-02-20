/** 
 **************************************************************
 * @file mylib/s4451110_lta1000g.c
 * @author Sejin Son - 44511199
 * @date 02082023
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4451119_reg_lta1000g_init() - Initialise LED Bar GPIO pins as outputs.
 * lta1000g_seg_set(int segment, unsigned char countValue) - Set the value (0 or 1) for the corresponding LED Bar segment GPIO pin. (Internal function) 
 * s4451119_reg_lta1000g_write(unsigned short value) - Write the LED Bar segments high or low
 * s4451119_reg_lta1000g_lrotate(unsigned short value, int index) - 
 *************************************************************** 
 */




#include "board.h"
#include "debug_log.h"
#include "processor_hal.h"



/* 	
 * @Global variables
 * @param cursorOn = Giving a state of GPIO LED ON
 * @param cursorOff = Giving a state of GPIO LED OFF
 */
unsigned char cursorOn = 1;
unsigned char cursorOff = 0;

/*
 * Initialise all GPIO PIN for LED Segments as output by setting bit for all. 
 */
extern void s4451119_reg_lta1000g_init() {

    /* Enable the GPIO Clocks */
    __GPIOC_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();
	__GPIOG_CLK_ENABLE();
	__GPIOF_CLK_ENABLE();


	/* Initialise E6 as an output. */
	GPIOE->MODER &= ~(0x03 << (6 * 2));  
	GPIOE->MODER |= (0x01 << (6 * 2));   
	GPIOE->OSPEEDR &= ~(0x03<<(6 * 2));
	GPIOE->OSPEEDR |=   0x02<<(6 * 2);  
	GPIOE->OTYPER &= ~(0x01 << 6);       

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOE->PUPDR &= ~(0x03 << (6 * 2));   
	GPIOE->PUPDR |= ((0x01) << (6 * 2));  
	GPIOE->ODR |= (0x01 << 6);   
	

	/* Initialise G15 as an output. */
	GPIOG->MODER &= ~(0x03 << (15 * 2));  
	GPIOG->MODER |= (0x01 << (15 * 2));   
	GPIOG->OSPEEDR &= ~(0x03<<(15 * 2));
	GPIOG->OSPEEDR |=   0x02<<(15 * 2);  
	GPIOG->OTYPER &= ~(0x01 << 15);       

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOG->PUPDR &= ~(0x03 << (15 * 2));   
	GPIOG->PUPDR |= ((0x01) << (15 * 2));  
	GPIOG->ODR &= (0x01 << 15);   

	/* Initialise G13 as an output. */
	GPIOG->MODER &= ~(0x03 << (13 * 2));  
	GPIOG->MODER |= (0x01 << (13 * 2));   
	GPIOG->OSPEEDR &= ~(0x03<<(13 * 2));
	GPIOG->OSPEEDR |=   0x02<<(13 * 2);  
	GPIOG->OTYPER &= ~(0x01 << 13);      

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOG->PUPDR &= ~(0x03 << (13 * 2));   
	GPIOG->PUPDR |= ((0x01) << (13 * 2));  
	GPIOG->ODR &= (0x01 << 13);   

	/* Initialise G10 as an output. */
	GPIOG->MODER &= ~(0x03 << (10 * 2));  
	GPIOG->MODER |= (0x01 << (10 * 2));   
	GPIOG->OSPEEDR &= ~(0x03<<(10 * 2));
	GPIOG->OSPEEDR |=   0x02<<(10 * 2);  
	GPIOG->OTYPER &= ~(0x01 << 10);       

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOG->PUPDR &= ~(0x03 << (10 * 2));   
	GPIOG->PUPDR |= ((0x01) << (10 * 2));  
	GPIOG->ODR &= (0x01 << 10);   
	
	/* Initialise G11 as an output. */
	GPIOG->MODER &= ~(0x03 << (11 * 2));  
	GPIOG->MODER |= (0x01 << (11 * 2));   
	GPIOG->OSPEEDR &= ~(0x03<<(11 * 2));
	GPIOG->OSPEEDR |=   0x02<<(11 * 2);  
	GPIOG->OTYPER &= ~(0x01 << 11);       

	/*  Activate the Pull-up or Pull down resistor for the current IO */
	GPIOG->PUPDR &= ~(0x03 << (11 * 2));  
	GPIOG->PUPDR |= ((0x01) << (11 * 2));  
	GPIOG->ODR &= (0x01 << 11);   

	/* Initialise F11 as an output. */
	GPIOF->MODER &= ~(0x03 << (11 * 2));  
	GPIOF->MODER |= (0x01 << (11 * 2));   
	GPIOF->OSPEEDR &= ~(0x03<<(11 * 2));
	GPIOF->OSPEEDR |=   0x02<<(11 * 2);  
	GPIOF->OTYPER &= ~(0x01 << 11);  

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOF->PUPDR &= ~(0x03 << (11 * 2));   
	GPIOF->PUPDR |= ((0x01) << (11 * 2));  
	GPIOF->ODR &= (0x01 << 11);   

	/* Initialise E0 as an output. */
	GPIOE->MODER &= ~(0x03 << (0 * 2));  
	GPIOE->MODER |= (0x01 << (0 * 2));   
	GPIOE->OSPEEDR &= ~(0x03<<(0 * 2));
	GPIOE->OSPEEDR |=   0x02<<(0 * 2);  
	GPIOE->OTYPER &= ~(0x01 << 0);       

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOE->PUPDR &= ~(0x03 << (0 * 2));   
	GPIOE->PUPDR |= ((0x01) << (0 * 2));  
	GPIOE->ODR &= (0x01 << 0);   

	/* Initialise G8 as an output. */
	GPIOG->MODER &= ~(0x03 << (8 * 2));  
	GPIOG->MODER |= (0x01 << (8 * 2));   
	GPIOG->OSPEEDR &= ~(0x03<<(8 * 2));
	GPIOG->OSPEEDR |=   0x02<<(8 * 2);  
	GPIOG->OTYPER &= ~(0x01 << 8);       

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOG->PUPDR &= ~(0x03 << (8 * 2));   
	GPIOG->PUPDR |= ((0x01) << (8 * 2));  
	GPIOG->ODR &= (0x01 << 8);   

	/* Initialise G5 as an output. */
	GPIOG->MODER &= ~(0x03 << (5 * 2));  
	GPIOG->MODER |= (0x01 << (5 * 2));   
	GPIOG->OSPEEDR &= ~(0x03<<(5 * 2));
	GPIOG->OSPEEDR |=   0x02<<(5 * 2);  
	GPIOG->OTYPER &= ~(0x01 << 5);      

	/* Activate the Pull-up or Pull down resistor for the current IO */
	GPIOG->PUPDR &= ~(0x03 << (5 * 2));   
	GPIOG->PUPDR |= ((0x01) << (5 * 2));  
	GPIOG->ODR &= (0x01 << 5);   

	/* Initialise G6 as an output. */
	GPIOG->MODER &= ~(0x03 << (6 * 2));  
	GPIOG->MODER |= (0x01 << (6 * 2));   
	GPIOG->OSPEEDR &= ~(0x03<<(6 * 2));
	GPIOG->OSPEEDR |=   0x02<<(6 * 2);  
	GPIOG->OTYPER &= ~(0x01 << 6);      

	/*  Activate the Pull-up or Pull down resistor for the current IO */
	GPIOG->PUPDR &= ~(0x03 << (6 * 2));   
	GPIOG->PUPDR |= ((0x01) << (6 * 2));  
	GPIOG->ODR &= (0x01 << 6);   
	
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
extern void lta1000g_seg_set(int segment, unsigned char countValue) {
	/*
	 * Switch cases to set corresponding values to the registers of LED to turn on/off
	 */
	 switch (segment) {

		/* LED 1 (C6) */
        case 1:

            if (countValue == 1) {

                GPIOE->ODR |= (0x01 << 6);	

            } else {

                GPIOE->ODR &= ~(0x01 << 6);

            }
            break;

		/* LED 2 (B15) */
        case 2:
            if (countValue == 1) {

                GPIOG->ODR |= (0x01 << 15);

            } else {

                GPIOG->ODR &= ~(0x01 << 15);

			}
            break;

		/* LED 3 (B13) */
        case 3:

            if (countValue == 1) {

                GPIOG->ODR |= (0x01 << 10);

            } else {

                GPIOG->ODR &= ~(0x01 << 10);

            }
            break;

		/* LED 4 (B12) */	
        case 4:

            if (countValue == 1) {

                GPIOG->ODR |= (0x01 << 13);

            } else {

                GPIOG->ODR &= ~(0x01 << 13);

            }
            break;

		/* LED 5 (A15) */
        case 5:

            if (countValue == 1) {

                GPIOG->ODR |= (0x01 << 11);

            } else {

                GPIOG->ODR &= ~(0x01 << 11);

            }
            break;

		/* LED 6 (C7) */	
        case 6:

            if (countValue == 1) {

                GPIOF->ODR |= (0x01 << 11);

            } else {

                GPIOF->ODR &= ~(0x01 << 11);

            }
            break;

		/* LED 7 (B5) */
        case 7:

            if (countValue == 1) {

                GPIOE->ODR |= (0x01 << 0);

            } else {

                GPIOE->ODR &= ~(0x01 << 0);

            }
            break;

		/* LED 8 (B3) */	
        case 8:

            if (countValue == 1) {

                GPIOG->ODR |= (0x01 << 8);

            } else {

                GPIOG->ODR &= ~(0x01 << 8);

			}
			break;

		/* LED 9 (A4) */	
		case 9:

		    if (countValue == 1) {

                GPIOG->ODR |= (0x01 << 5);

            } else {

                GPIOG->ODR &= ~(0x01 << 5);

			}
			break;

		/* LED 10 (B4) */	
		case 10:

		    if (countValue == 1) {

                GPIOG->ODR |= (0x01 << 6);

            } else {

                GPIOG->ODR &= ~(0x01 << 6);

			}
			break;
	 }	

}

/*
 * This function writes to the LED Bar segments, setting them high or low based 
 * on the value of countValue. It uses a bitmask to check each bit of countValue,
 * iterating through each bit, and setting the corresponding LED segment accordingly.
 *
 * @param countValue : The value to be written to the LED Bar. Each bit corresponds 
 *                     to an LED segment.
 * @variable mask : A mask created by shifting 1 to the left by 'i' places. Used to 
 *                  isolate a specific bit in countValue.
 * @variable bitIsSet : A boolean representing whether the isolated bit in countValue
 *                      is set (1) or not set (0).
 */
extern void s4451119_reg_lta1000g_write(unsigned short countValue) {

    for(int i = 0; i < 10; i++) {

        unsigned short mask = 1 << i;
        unsigned char bitIsSet = (countValue & mask) > 0;
        lta1000g_seg_set(i+1, bitIsSet);

    }
}


/*
 * This function performs a left rotation on the given value, shifting the bits 
 * by the specified index. It then writes the rotated value to the LED Bar.
 * 
 * The function ensures the index is within the valid range (0 to ledCount - 1).
 * If it is out of range, it is set to either 0 or ledCount - 1, whichever is closer.
 * 
 * @param value : The value to be rotated and written to the LED Bar.
 * @param index : The number of positions to rotate the bits to the left.
 * @variable rotatedValue : The resulting value after rotating 'value' by 'index'
 *                          positions to the left. This value is then written to the LED Bar.
 * @return : The value after rotation.
 */
extern unsigned short s4451119_reg_lta1000g_lrotate(unsigned short value, int index) {

    const int ledCount = 10;

    if (index < 0) {

        index = 0;

    } else if (index > ledCount - 1) {

        index = ledCount - 1;

    }

    unsigned short rotatedValue = ((value << index) | (value >> (ledCount - index))) & 0x3FF;
    s4451119_reg_lta1000g_write(rotatedValue);
    return rotatedValue;
	
}