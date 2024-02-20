/** 
 **************************************************************
 * @file mylib/s4451119_hamming.h
 * @author Sejin Son - 44511199
 * @date 23082023
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * extern unsigned short s4451119_lib_hamming_byte_encode(unsigned char value) - Implement Hamming Code on a full byte of input, This means that 16-bits of output are needed.
 * extern unsigned short s4451119_lib_hamming_hbyte_encode(unsigned char value) - Implement Hamming Code on a half byte of input. Convert a 4-bit input into an 8-bit output
 * extern unsigned char s4451119_lib_hamming_byte_decode(unsigned char value) - Implement Hamming decode on a full byte of input. This means that 4-bits of output are produced.
 *************************************************************** 
 */


#include "processor_hal.h"
#include "board.h"

extern unsigned short s4451119_lib_hamming_byte_encode(unsigned char value);
extern unsigned char s4451119_lib_hamming_hbyte_encode(unsigned char value);
extern unsigned char s4451119_lib_hamming_byte_decode(unsigned char value);
int s4451119_lib_hamming_parity_error(unsigned char codedWord);