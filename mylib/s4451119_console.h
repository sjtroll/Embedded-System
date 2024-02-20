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


#define CONSOLE_EVENT_Q			1 << 0		
#define CONSOLE_EVENT_W			1 << 1
#define CONSOLE_EVENT_E		    1 << 2		
#define CONSOLE_EVENT_1		  	1 << 3		
#define CONSOLE_EVENT_A		    1 << 4		
#define CONSOLE_EVENT_S		  	1 << 5		
#define CONSOLE_EVENT_D		    1 << 6		
#define CONSOLE_EVENT_2			1 << 7		
#define CONSOLE_EVENT_Z		    1 << 8		
#define CONSOLE_EVENT_X			1 << 9		
#define CONSOLE_EVENT_C		    1 << 10		
#define CONSOLE_EVENT_3		    1 << 11		
#define CONSOLE_EVENT_4		    1 << 12		
#define CONSOLE_EVENT_5		    1 << 13		
#define CONSOLE_EVENT_6		    1 << 14		
#define CONSOLE_EVENT_7		    1 << 15		

#define CONSOLE_EVENT		    0xFFFF	    //Control Event Group Mask

EventGroupHandle_t consoleEventGroup;

void s4451119_console_task();
