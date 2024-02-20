 /** 
 **************************************************************
 * @file mylib/s4451119_sysmon.h
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

void s4451119_asccont_taskinit();
void asccont_console_task();
void asccont_join_task();
