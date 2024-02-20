/** 
 **************************************************************
 * @file mylib/s4451119_txradio.c
 * @author Sejin Son - 44511199
 * @date 10/09/23
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4451119_txradio_init( void ) - Initialisation function for the NRFl2401Plus Radio Transmitter.
 * s4451119_txradio_task_control_radio() - Helper function to construct packets ready for transmission.
 * txradio_fsmprocessing() - Finite State Machine (FSM) for processing the radio transmission.
 * construct_tx_packet() - Task to control the radio transmission.
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "debug_log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
#include <string.h>
#include "s4451119_hamming.h"
#include "s4451119_txradio.h"
#include "nrf24l01plus.h"


/* Global variables for packet handling and radio transmission */
Packet* packet;
int rawPacket[16];
uint8_t codedPacket[32];
/* Queue used for radio transmission */
QueueHandle_t radioTxQueue;
/* Address of the sender */
uint8_t senderAddress[] = {0x44, 0x51, 0x11, 0x99};

/* 
 * Initialisation function for the NRFl2401Plus Radio Transmitter.
 * Initializes the NRF24L01+ radio and creates a queue for packets.
 */
void s4451119_txradio_init() {
    nrf24l01plus_init();
	radioTxQueue = xQueueCreate(30, sizeof(Packet));
}

/* 
 * Helper function to convert an integer value to its ASCII representation.
 * The conversion starts from the specified start index and fills the specified number of digits.
 */
static void int_to_ascii(int value, int start, int digits) {
    for (int i = start + digits - 1; i >= start; i--) {
        rawPacket[i] = (value % 10) + '0';
        value /= 10;
    }
}

/* 
 * Helper function to construct packets ready for transmission.
 * Packets are first constructed in BigEndian byte order then reversed at the end.
 * The function uses the global packet structure to construct the raw packet.
 */
void construct_tx_packet() {
	
	int type = packet->type;
	int payLoadLength = strlen(packet->payLoadString);

	/* Copy sender address to the raw packet */
	for (int i = 0; i < SENDER_ADDRESS_SIZE; i++) {
    	rawPacket[1 + i] = senderAddress[i];
	}
    /* Copy payload string to the raw packet */
    for (int i = 0; i < payLoadLength; i++) {
        rawPacket[PAYLOAD_OFFSET + i] = packet->payLoadString[i];
    }

    /* Default padding for the remaining bytes in the raw packet */
    for (int i = PAYLOAD_OFFSET + payLoadLength; i < 16; i++) {
        rawPacket[i] = 0x0;
    }

    /* Construct the packet based on its type */
    switch (packet->type) {
        case 0: 
            rawPacket[0] = 0x20; 
            break;
        case 1:
            rawPacket[0] = 0x22;
            /* Convert XYZ coordinates to ASCII and store in the raw packet */
            for (int i = 0; i < 3; i++) {
                int digits = (i == 2) ? 2 : 3;  // Z has only 2 digits
                int_to_ascii(packet->xyz[i], XYZ_OFFSET + i * 3, digits);
            }
            break;
        case 2:
            rawPacket[0] = 0x23;
            /* Convert rotation value to ASCII and store in the raw packet */
            int_to_ascii(packet->rot, ROT_OFFSET, 3);
            break;
        case 3: 
            rawPacket[0] = 0x24; 
            break;
    }
}

/* 
 * Finite State Machine (FSM) for processing the radio transmission.
 * The FSM has three states: QUEUEVALIDATION, PACKETMAKE, and PACKETRANSMIT.
 * - QUEUEVALIDATION: Validates the existence of the radioTxQueue and checks for packets.
 * - PACKETMAKE: Constructs the packet, swaps nibbles, and encodes the packet.
 * - PACKETRANSMIT: Sends the encoded packet using the radio and toggles the LED.
 */
void txradio_fsmprocessing() {
	switch(radioState) {
		case QUEUEVALIDATION:
			/* Check if the radioTxQueue exists */
			if (radioTxQueue != NULL) {
				/* If a packet is received from the queue, move to the PACKETMAKE state */
				if (xQueueReceive(radioTxQueue, &packet, 10 ) == pdTRUE) {
					radioState = PACKETMAKE;
				}
			}
			break;
		case PACKETMAKE:
			construct_tx_packet();
			/* Swap the upper and lower nibbles of each byte in rawPacket */
			for (int i = 0; i < PACKET_SIZE; i++) {
				rawPacket[i] = (rawPacket[i] >> 4) | (rawPacket[i] << 4);
			}

			/* Encode each byte in rawPacket and store the result in codedPacket */
			for (int i = 0; i < ENCODED_PACKET_SIZE; i += 2) {
				unsigned short encodedByte = s4451119_lib_hamming_byte_encode(rawPacket[i / 2]);
				codedPacket[i] = encodedByte >> 8;
				codedPacket[i + 1] = encodedByte;
			}
			radioState = PACKETRANSMIT;
			break;
		case PACKETRANSMIT:
			vTaskDelay(50);
			taskENTER_CRITICAL();

			/* Toggle the blue LED to indicate transmission */
			BRD_LEDBlueToggle();
			/* Send the encoded packet using the radio */
			nrf24l01plus_send(codedPacket);
			/* Free the memory allocated for the packet */
			vPortFree(packet);
			/* Return to the QUEUEVALIDATION state */
			radioState = QUEUEVALIDATION;
			
			taskEXIT_CRITICAL();
			break;
	}
}

/* 
 * Task to control the radio transmission.
 * This task initializes the radio and continuously processes the FSM for radio transmission.
 */
void s4451119_txradio_task_control_radio() {

	/* Enter critical section to safely initialize the radio and set the initial state */
	taskENTER_CRITICAL();
	s4451119_txradio_init();
	radioState = QUEUEVALIDATION;
	taskEXIT_CRITICAL();

	for (;;) {
		/* Process the FSM for radio transmission */
		txradio_fsmprocessing();
		vTaskDelay(15);
	}
}