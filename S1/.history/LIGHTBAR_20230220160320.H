 /** 
 **************************************************************
 * @file mylib/s4451110_lta1000g.h
 * @author Sejin Son - 44511199
 * @date 22022022
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4451119_reg_lta1000g_init() - Initialise LED Bar GPIO pins as outputs.
 * lta1000g_seg_set(int segment, int bit) - Set the value (0 or 1) for the corresponding LED Bar segment GPIO pin. (Internal function) 
 * s4451119_reg_lta1000g_write() - Write the LED Bar segments high or low
 *************************************************************** 
 */

extern void s4451119_reg_lta1000g_init();
int lta1000g_seg_set(int segment, int bit);
extern void s4451119_reg_lta1000g_write();