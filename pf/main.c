/**
 ******************************************************************************
 * @file    repo/pf/main.c
 * @author  Sejin Son
 * @date    30/09/23
 * @brief   Project main.c
 ******************************************************************************
 *
 */
#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "debug_log.h"
#include "s4451119_asccont.h"


/*
 * Init devics and debug tools and all related tasks by the function s4451119_asccont_run()
 */
int main( void ) {
	
	HAL_Init();

  	BRD_debuguart_init();
  	s4451119_asccont_taskinit();

  	return 0;
}