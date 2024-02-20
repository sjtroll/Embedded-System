 /** 
 **************************************************************
 * @file mylib/s4451119_console.h
 * @author Sejin Son - 44511199
 * @date 10/09/23
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4451119_console_task() - istens for characters from the UART console and sets the corresponding event bits
 *************************************************************** 
 */


#include "board.h"
#include "processor_hal.h"
#include "debug_log.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "s4451119_console.h"

/* Event group handle for console events */
EventGroupHandle_t consoleEventGroup;

/* Timeout for UART character reception in milliseconds */
#define UART_RECV_TIMEOUT 10

/* 
 * Task function to handle console input.
 * This task listens for characters from the UART console and sets the corresponding event bits
 * in the consoleEventGroup based on the received character.
 */
void s4451119_console_task() {
    /* Enter critical section to safely create the event group */
    taskENTER_CRITICAL();
    consoleEventGroup = xEventGroupCreate();
    taskEXIT_CRITICAL();

    /* Lookup table for character to event bit mapping */
    struct CharEventMapping {
        char character;
        EventBits_t eventBit;
    };

    struct CharEventMapping lookupTable[] = {
        {'Q', CONSOLE_EVENT_Q},
        {'W', CONSOLE_EVENT_W},
        {'E', CONSOLE_EVENT_E},
        {'1', CONSOLE_EVENT_1},
        {'A', CONSOLE_EVENT_A},
        {'S', CONSOLE_EVENT_S},
        {'D', CONSOLE_EVENT_D},
        {'2', CONSOLE_EVENT_2},
        {'Z', CONSOLE_EVENT_Z},
        {'X', CONSOLE_EVENT_X},
        {'C', CONSOLE_EVENT_C},
        {'3', CONSOLE_EVENT_3},
        {'4', CONSOLE_EVENT_4},
        {'5', CONSOLE_EVENT_5},
        {'6', CONSOLE_EVENT_6},
        {'7', CONSOLE_EVENT_7},
    };

    TickType_t startTime, currentTime;

    for (;;) {
        /* Get the start time for UART reception */
        startTime = xTaskGetTickCount();
        unsigned char recvChar = BRD_debuguart_getc();
        /* Get the current time after UART reception */
        currentTime = xTaskGetTickCount();

        /* Check if a valid character was received within the timeout period */
        if (recvChar != '\0' && (currentTime - startTime) <= UART_RECV_TIMEOUT) {
            /* Toggle the red LED to indicate character reception */
            BRD_LEDRedToggle();
            size_t i;
            /* Search the lookup table for the received character and set the corresponding event bit */
            for(i = 0; i < sizeof(lookupTable) / sizeof(lookupTable[0]); i++) {
                if (recvChar == lookupTable[i].character) {
                    xEventGroupSetBits(consoleEventGroup, lookupTable[i].eventBit);
                    break;
                }
            }
        }

        /* Delay to allow other tasks to run */
        vTaskDelay(10);
    }
    /* Cleanup (if the task is ever terminated) */
    vEventGroupDelete(consoleEventGroup);
}