/**
**************************************************************
* @file mylib/s4451119_sysmon.c
* @author Sejin Son - 44511199
* @date 10/09/23
* @brief mylib template driver
* REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* s4451119_asccont_taskinit() - Initialization function for the ASC controller.
* asccont_console_task() - Task function for handling console-related operations.
* asccont_join_task() - Task function for handling join operations.
***************************************************************
*/

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "debug_log.h"
#include "semphr.h"
#include "event_groups.h"

#include "s4451119_console.h"
#include "s4451119_hamming.h"
#include "s4451119_txradio.h"
#include "s4451119_asccont.h"
#include "s4451119_ascext.h"
#include "s4451119_display.h"


#define RADIO_TASK_PRIORITY (tskIDLE_PRIORITY + 3)
#define CONSOLE_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define ASCCONT_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define DISPLAY_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define ASCJOIN_TASK_PRIORITY (tskIDLE_PRIORITY + 2)



#define ASCJOIN_TASK_STACK_SIZE ((configMINIMAL_STACK_SIZE * 4))
#define CONSOLE_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 8)
#define DISPLAY_TASK_STACK_SIZE ((configMINIMAL_STACK_SIZE * 64))
#define ASCCONT_TASK_STACK_SIZE ((configMINIMAL_STACK_SIZE * 16))
#define RADIO_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 8)




TaskHandle_t join_task_handle;
TaskHandle_t console_task_handle;
TaskHandle_t asccont_console_task_handle;
TaskHandle_t txradio_task_handle;
TaskHandle_t display_task_handle;


SemaphoreHandle_t packetSemaphore; // Semaphore for Transmission Packets as they are pointers to structs.

int zCoordinate = 0;
int rotCoordinate = 0;
int xCoordinate = 0;
int yCoordinate = 0;
int vacuumOn = 0;

Coordinate lastCoordinate;
Packet *packet;

#define CONSOLE_EVENT 0xFFFF
#define XYZ_EVENT 0x0FFE
#define VAC_EVENT 0x0100
#define ROT_EVENT 0x1000
#define TRANS_EVENT (XYZ_EVENT | VAC_EVENT | ROT_EVENT)
#define BUTTON_PIN 13
#define DEBOUNCE_DELAY 1000
#define JOIN_PAYLOAD "JOIN"

void asccont_console_task()
{
    EventBits_t consoleEventBits;

    for (;;)
    {
        /* Ensure the console event group is initialized */
        if (consoleEventGroup != NULL) {
            /* Wait for specific console events */
            consoleEventBits = xEventGroupWaitBits(consoleEventGroup, CONSOLE_EVENT, pdTRUE, pdFALSE, 10);
            
            /* Handle transmission event */
            if (consoleEventBits & TRANS_EVENT) {
                xSemaphoreTake(packetSemaphore, 10);
                packet = pvPortMalloc(sizeof(Packet));
            }

            /* Handle XYZ coordinate event */
            if (consoleEventBits & XYZ_EVENT) {
                /* Set packet details for XYZ coordinates */
                packet->xyz[0] = xCoordinate;
                packet->xyz[1] = yCoordinate;
                packet->xyz[2] = zCoordinate;
                packet->rot = rotCoordinate;
                packet->type = 1;
                packet->payLoadString = "XYZ";
            }

            /* Mapping of console events to specific actions using a lookup table */
            typedef struct
            {
                EventBits_t event;
                int x;
                int y;
                char *payload;
                int type;
            } EventMapping;

            EventMapping mappings[] = {
                /* Define mappings for each console event to its corresponding action */
                {CONSOLE_EVENT_Q, 0, 150, "XYZ", -1},
                {CONSOLE_EVENT_W, 75, 150, "XYZ", -1},
                {CONSOLE_EVENT_E, 150, 150, "XYZ", -1},
                {CONSOLE_EVENT_A, 0, 75, "XYZ", -1},
                {CONSOLE_EVENT_S, 75, 75, "XYZ", -1},
                {CONSOLE_EVENT_D, 150, 75, "XYZ", -1},
                {CONSOLE_EVENT_Z, 0, 0, "XYZ", -1},
                {CONSOLE_EVENT_X, 75, 0, "XYZ", -1},
                {CONSOLE_EVENT_C, 150, 0, "XYZ", -1},
                {CONSOLE_EVENT_3, 0, 0, "ROT", 2},
                {CONSOLE_EVENT_4, 0, 0, vacuumOn ? "VOFF" : "VON", 3}
            };

            /* Iterate through the mappings to determine the action for the detected event */
            for (size_t i = 0; i < sizeof(mappings) / sizeof(EventMapping); i++) {
                if (consoleEventBits & mappings[i].event) {
                    lastCoordinate.x = mappings[i].x;
                    lastCoordinate.y = mappings[i].y;
                    if (mappings[i].type != -1) {
                        packet->type = mappings[i].type;
                        packet->payLoadString = mappings[i].payload;
                    }
                    break;
                }
            }

            /* Handle Z-coordinate adjustments based on console events */
            if (consoleEventBits & CONSOLE_EVENT_1 && zCoordinate > 0) {
                zCoordinate -= 10;
                lastCoordinate.z -= 10;
            }
            else if (consoleEventBits & CONSOLE_EVENT_2 && zCoordinate < 90) {
                zCoordinate += 10;
                lastCoordinate.z += 10;
            }

            /* Handle rotation adjustments based on console events */
            if (consoleEventBits & CONSOLE_EVENT_3) {
                rotCoordinate = (rotCoordinate + 10) % 190;
                lastCoordinate.rot = rotCoordinate;
            }

            /* Toggle vacuum state based on console event */
            if (consoleEventBits & CONSOLE_EVENT_4) {
                vacuumOn = !vacuumOn;
            }

            /* Update X and Y coordinates */
            xCoordinate = lastCoordinate.x;
            yCoordinate = lastCoordinate.y;

            /* Send packet to radio transmission queue if there's a transmission event */
            if (consoleEventBits & TRANS_EVENT) {
                xQueueSendToBack(radioTxQueue, (void *)&packet, (portTickType) 10);
                xSemaphoreGive(packetSemaphore);
            }
            
            /* Send the last coordinate to the display queue */
            xQueueSendToBack(displayQueue, ( void * ) &lastCoordinate, ( portTickType ) 10 );
        }
        vTaskDelay(75);
    }
}

/* 
 * Task to handle join-related events.
 * This task continuously checks for a button press event and sends a join packet upon detection.
 */
void asccont_join_task() {

    uint8_t lastButtonState = 0;  /* To track the previous state of the button */
    uint32_t previousTime = 0;

    /* Initialization: Enable GPIOC clock and configure button pin */
    __GPIOC_CLK_ENABLE();
    GPIOC->PUPDR &= ~(0x03 << (2 * BUTTON_PIN));
    GPIOC->PUPDR |= (0x00 << (2 * BUTTON_PIN));

    /* Pre-allocate memory for the packet */
    packet = pvPortMalloc(sizeof(Packet));

    for (;;) {
        uint32_t currentTime = HAL_GetTick();
        uint8_t currentButtonState = (GPIOC->IDR >> BUTTON_PIN) & 0x01;

        /* Check for button press with software debounce */
        if (!lastButtonState && currentButtonState) { /* Button just pressed */
            if (currentTime - previousTime > DEBOUNCE_DELAY) {
                if (xSemaphoreTake(packetSemaphore, 10) == pdTRUE) {
                    packet->type = 0;
                    packet->payLoadString = JOIN_PAYLOAD;
                    xQueueSendToBack(radioTxQueue, (void *)&packet, (portTickType)10);
                    xSemaphoreGive(packetSemaphore);
                }
                previousTime = currentTime;
            }
        }

        lastButtonState = currentButtonState;  /* Update the last button state */
        vTaskDelay(20);
    }
}



/*
 * Starter function called by Main.c that initialises all other tasks.
 * It is called by main.c
 */
void s4451119_asccont_taskinit()
{

    BRD_LEDInit(); // Initialise Blue LED
    Coordinate lastCoordinate = {0, 0, 0, 0};
    packetSemaphore = xSemaphoreCreateMutex();
    xSemaphoreGive(packetSemaphore);

  
    xTaskCreate((void *)&asccont_console_task, (const signed char *)"ASCCONTCONSOLE", ASCCONT_TASK_STACK_SIZE, NULL, ASCCONT_TASK_PRIORITY, &asccont_console_task_handle);
    xTaskCreate((void *)&s4451119_console_task, (const signed char *)"CONSOLEMAP", CONSOLE_TASK_STACK_SIZE, NULL, CONSOLE_TASK_PRIORITY, &console_task_handle);
    xTaskCreate((void *)&s4451119_display_task, (const signed char *)"DISPLAY", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PRIORITY, &display_task_handle);
    xTaskCreate((void *)&s4451119_txradio_task_control_radio, (const signed char *)"RADIO", RADIO_TASK_STACK_SIZE, NULL, RADIO_TASK_PRIORITY, &txradio_task_handle);
    xTaskCreate((void *)&asccont_join_task, (const signed char *)"JOIN", ASCJOIN_TASK_STACK_SIZE, NULL, ASCJOIN_TASK_PRIORITY, &join_task_handle);

    
    vTaskStartScheduler();
}