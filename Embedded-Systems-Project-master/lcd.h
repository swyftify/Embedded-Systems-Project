#ifndef LCD_H
#define LCD_H
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void vStartLcd( unsigned portBASE_TYPE uxPriority, xQueueHandle);

#endif
