/** 
 **************************************************************
 * @file mylib/s4451119_txradio.h
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

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"

typedef struct {
	int type;
	int rot;
	int xyz[3];
	char* payLoadString;
} Packet; 

static int radioState;

#define QUEUEVALIDATION 1
#define PACKETMAKE 2
#define PACKETRANSMIT 3

#define SENDER_ADDRESS_SIZE 4
#define PAYLOAD_OFFSET 5
#define XYZ_OFFSET 8
#define ROT_OFFSET 8
#define PACKET_SIZE 16
#define ENCODED_PACKET_SIZE 32


QueueHandle_t radioTxQueue;	// Queue used

void s4451119_txradio_init();
void s4451119_txradio_task_control_radio();
void txradio_fsmprocessing();
void construct_tx_packet();

