/**
 **************************************************************
 * @file mylib/s4451119_hamming.c
 * @author Sejin Son - 44511199
 * @date 28082023
 * @brief The Hamming LIB provides hamming byte encode, byte decode and half byte encode functions
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4451119_lib_hamming_byte_encode(unsigned char in) - encoding hamming (7,4) words given, 8-bit and output 16 bit
 * s4451119_lib_hamming_hbyte_decode(unsigned char in) - decoding hamming (7,4) words given, 8-bit and output 4 bit
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "debug_log.h"
#include "s4451119_lta1000g.h"
#include "s4451119_joystick.h"
#include "s4451119_hamming.h"



/* 
 * @breif   Implement Hamming Code on a full byte of input, This means that 16-bits of output are needed.
 * @param   inputWord - Input word for encoding, 8-bit length.
 * @return  encodedWord - Encoded word by combining two half-byte encodings, 16-bit length.
 */
unsigned short s4451119_lib_hamming_byte_encode(unsigned char inputWord) {
    unsigned short encodedWord;
	unsigned short upper;
	unsigned short lower;

	/* 
	 * first encode D0..D3 (first 4 bits),
	 * then D4..D7 (second 4 bits).
	 */
	upper = s4451119_lib_hamming_hbyte_encode(inputWord & 0xF);
	lower = (s4451119_lib_hamming_hbyte_encode(inputWord >> 4) << 8);

	encodedWord = (upper | lower);

	return (encodedWord);
}


/* 
 * @brief   Implement Hamming Code on a half byte of input. Convert a 4-bit input into an 8-bit output 
 *          consisting of 4 data bits, 3 hamming bits, and 1 parity bit.
 * @param   halfInput - Half byte of inputWord (4-bit).
 * @return  out - Encoded word using hamming (7,4) code, 8-bit length.
 */
unsigned char s4451119_lib_hamming_hbyte_encode(unsigned char halfInput) {
    unsigned char d0, d1, d2, d3;
	unsigned char p0 = 0, h0, h1, h2;
	unsigned char out;
	uint8_t z;

	/* extract bits */
	d0 = !!(halfInput & 0x1);
	d1 = !!(halfInput & 0x2);
	d2 = !!(halfInput & 0x4);
	d3 = !!(halfInput & 0x8);
	
	/* calculate hamming parity bits */
	h0 = d1 ^ d2 ^ d3;
	h1 = d0 ^ d2 ^ d3;
	h2 = d0 ^ d1 ^ d3;

	/* generate out byte without parity bit P0 */
	out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
		(d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);


	/* calculate even parity bit */
	for (z = 1; z < 8; z++)
		p0 = p0 ^ !!(out & (1 << z));

	out |= p0;

	return(out);

}

/* 
 * @brief   Implement Hamming decode on a full byte of input. This means that 4-bits of output are produced.
 * @param   inputWord - Input word for decoding, 8-bit length.
 * @return  correctedWord - Decoded word after correcting for possible errors, 4-bit length.
 */
unsigned char s4451119_lib_hamming_byte_decode(unsigned char inputWord) {

	unsigned char correctedWord;
	unsigned char codedWord;
	uint8_t s0,s1,s2;
	uint8_t syndrome;
	uint8_t error;

	codedWord = s4451119_lib_hamming_hbyte_encode(inputWord & 0xF);
	error = s4451119_lib_hamming_parity_error(codedWord);
	
	// Check for errors.
	if (error) {

		// Correct the error
		s0 = !!(codedWord & 0x2) ^ !!(codedWord & 0x20) ^ !!(codedWord & 0x40) ^ !!(codedWord & 0x80);
		s1 = !!(codedWord & 0x4) ^ !!(codedWord & 0x10) ^ !!(codedWord & 0x40) ^ !!(codedWord & 0x80);
		s2 = !!(codedWord & 0x8) ^ !!(codedWord & 0x10) ^ !!(codedWord & 0x20) ^ !!(codedWord & 0x80);

		syndrome = (s0 << 2) | (s1 << 1) | s2;

		if (syndrome) {
			codedWord ^= (1 << (syndrome - 1)); // Flip the erroneous bit
		}
	}


	for (int i = 8; i < 10; i++) {
        unsigned char bitValue = (error >> i) & 0x01;
        lta1000g_seg_set(i + 1, bitValue);
    }
	correctedWord = (codedWord & 0xF0) >> 4;
	// Return the decoded 4-bit data
	return correctedWord;
}



/* 
 * @brief   Check for parity errors in the given encoded word using Hamming (7,4) code.
 * @param   codedWord - 8-bit word that was encoded using Hamming (7,4) code.
 * @return  Returns 1 if there's an error, otherwise 0.
 */
int s4451119_lib_hamming_parity_error(unsigned char codedWord) {

	uint8_t d0,d1,d2,d3,h0,h1,h2,p0,pd;
	uint8_t s0,s1,s2;
	uint8_t syndrome;
	uint8_t parity_check;
	uint8_t errorReturn;

	// Extract bits from the word
	p0 = !!(codedWord & 0x1);
	h0 = !!(codedWord & 0x2);
	h1 = !!(codedWord & 0x4);
	h2 = !!(codedWord & 0x8);
	d0 = !!(codedWord & 0x10);
	d1 = !!(codedWord & 0x20);
	d2 = !!(codedWord & 0x40);
	d3 = !!(codedWord & 0x80);

	// Calculate syndrome bits
	s0 = h0 ^ d1 ^ d2 ^ d3;
	s1 = h1 ^ d0 ^ d2 ^ d3;
	s2 = h2 ^ d0 ^ d1 ^ d3;

	syndrome = (s0 << 2) | (s1 << 1) | s2;

	// Check the overall parity bit
	parity_check = d0 ^ d1 ^ d2 ^ d3 ^ h0 ^ h1 ^ h2;

	errorReturn = (syndrome || (parity_check != p0));

	// Return 1 if there's an error, 0 otherwise
	return errorReturn ? 0x1 : 0;
}

