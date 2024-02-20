 /** 
 **************************************************************
 * @file mylib/s4451119_display.h
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

#include "processor_hal.h"
#include "board.h"
#include "debug_log.h"

QueueHandle_t displayQueue;

 typedef struct {
  int x;
  int y;
  int z;
  int rot;
} Coordinate;

void s4451119_display_draw();
void s4451119_display_task();
