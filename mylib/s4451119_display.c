/** 
 **************************************************************
 * @file mylib/s4451119_display.c
 * @author Sejin Son - 44511199
 * @date 10/09/23
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4451119_display_draw(Coordinate sorterHead) - Function to draw the display based on the sorter's head position.
 * s4451119_display_task() - Task to handle the display updates.
 *************************************************************** 
 */


#include "board.h"
#include "debug_log.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
#include "s4451119_display.h"
#include "s4451119_lta1000g.h"

QueueHandle_t displayQueue;

#define SCREEN_CLEAR()  debug_log("\e[2J")
#define SCREEN_HOME()  debug_log("\e[H")

#define FG_BLACK 	30
#define FG_RED		31
#define FG_GREEN	32
#define FG_YELLOW	33
#define FG_BLUE		34
#define FG_MAGENTA	35
#define FG_CYAN		36
#define FG_WHITE	37
#define BG_BLACK	40
#define BG_RED		41
#define BG_GREEN	42
#define BG_YELLOW	43
#define BG_BLUE		44
#define BG_MAGENTA	45
#define BG_CYAN		46
#define BG_WHITE	47

#define CELL_BLACK "\e[7;30;40m"
#define CELL_RED	"\e[7;31;41m"
#define CELL_GREEN	"\e[7;32;42m"
#define CELL_YELLOW		"\e[7;33;43m"
#define CELL_BLUE		"\e[7;34;44m"
#define CELL_MAGENTA	"\e[7;35;45m"
#define CELL_CYAN		"\e[7;36;46m"
#define CELL_WHITE 		"\e[7;37;47m"

#define CURSOR_DOWN_COLOR "\e[32m"  // Green
#define CURSOR_UP_COLOR "\e[34m"    // Blue
#define RESET_COLOR "\e[0m"


char *cell_colour_palatte[] = {CELL_BLACK, CELL_RED, CELL_GREEN, CELL_YELLOW, CELL_BLUE, CELL_MAGENTA, CELL_CYAN, CELL_WHITE};
#define SET_CELL_COLOUR(fg, bg) debug_log("\e[7;%d;%dm",fg,bg)

/* 
 * Function to draw the display based on the sorter's head position.
 * The display represents the sorter's movement in the X and Y axes, 
 * and the color indicates the movement in the Z axis.
 * 
 * @param sorterHead: The current position of the sorter's head.
 */
void s4451119_display_draw(Coordinate sorterHead) {
    /* Clear the screen and reset the cursor to the home position */
    SCREEN_CLEAR();

    /* Calculate the position of the sorter's head on the display using a scaling factor */
    int displayX = sorterHead.x / 8;
    int displayY = 25 - (sorterHead.y / 8);

    /* Static variable to remember the previous Z-coordinate for comparison */
    static int prevZ = 0;

    /* Determine the direction of Z movement and set the appropriate color */
    char* color;
    if (sorterHead.z > prevZ) {
        color = CURSOR_UP_COLOR;  /* Blue indicates upward movement */
    } else if (sorterHead.z < prevZ) {
        color = CURSOR_DOWN_COLOR;  /* Green indicates downward movement */
    } else {
        color = RESET_COLOR;  /* Default color for no change in Z movement */
    }

    /* Update the previous Z-coordinate for future comparisons */
    prevZ = sorterHead.z;

    /* Draw the bounding box and the sorter's head on the display */
    for (int j = 0; j < 27; j++) {
        for (int i = 0; i < 27; i++) {
            if (j == 0 || j == 26 || i == 0 || i == 26) {
                debug_log("#");
            } else if (i == displayX + 1 && j == displayY + 1) {
                debug_log("%s+%s", color, RESET_COLOR);  /* Display the sorter's head using '+' and the determined color */
            } else {
                debug_log(" ");
            }
        }
        debug_log("\n\r");
    }
}

/* 
 * Task to handle the display updates.
 * This task continuously checks the displayQueue for new sorter positions 
 * and updates the display accordingly.
 */
void s4451119_display_task() {
	
    /* Enter critical section to ensure safe queue creation */
    taskENTER_CRITICAL();
    Coordinate sorter;
    displayQueue = xQueueCreate(10, sizeof(sorter));	
    taskEXIT_CRITICAL();

    for(;;) {
        if (displayQueue != NULL) {
            /* Receive the new sorter position from the displayQueue */
            if (xQueueReceive(displayQueue, &sorter, 20) == pdTRUE) {
                /* Enter critical section to ensure safe display update */
                taskENTER_CRITICAL();
                s4451119_display_draw(sorter);  /* Update the display with the new sorter position */
                taskEXIT_CRITICAL();
            }
        }
        vTaskDelay(500);  /* Delay for 500ms before checking the queue again */
    }   
}