 /** 
 **************************************************************
 * @file mylib/s4451110_lta1000g.h
 * @author Sejin Son - 44511199
 * @date 02082023
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * extern void s4451119_reg_lta1000g_init() - Initialise LED Bar GPIO pins as outputs.
 * int lta1000g_seg_set(int segment, unsigned char countValue) - Set the value (0 or 1) for the corresponding LED Bar segment GPIO pin. (Internal function) 
 * extern void s4451119_reg_lta1000g_lrotate(unsigned short countcountValue, unsigned short mask)
 * extern void s4451119_reg_lta1000g_write(unsigned short countValue) 
 *************************************************************** 
 */

extern void s4451119_reg_lta1000g_init();
extern void s4451119_reg_lta1000g_write(unsigned short value);
extern unsigned short s4451119_reg_lta1000g_lrotate(unsigned short value, int index);
extern void lta1000g_seg_set(int segment, unsigned char countValue);