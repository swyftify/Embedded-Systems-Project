#ifndef SENSORS_H
#define SENSORS_H


static xQueueHandle globalInterfaceQueue;

void vStartSensors( unsigned portBASE_TYPE uxPriority, xQueueHandle);

#endif
