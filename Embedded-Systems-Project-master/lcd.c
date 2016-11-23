/* 
	Sample task that initialises the EA QVGA LCD display
	with touch screen controller and processes touch screen
	interrupt events.

	Jonathan Dukes (jdukes@scss.tcd.ie)
*/

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lcd.h"
#include "lcd_hw.h"
#include "lcd_grph.h"
#include <stdio.h>
#include <string.h>
#include "UI.h"

/* Maximum task stack size */
#define lcdSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )
	
void drawDefaultUI();

/* Interrupt handlers */
extern void vLCD_ISREntry( void );
void vLCD_ISRHandler( void );

/* The LCD task. */
static void vLcdTask( void *pvParameters );

/* Semaphore for ISR/task synchronisation */
xSemaphoreHandle xLcdSemphr;

void vStartLcd( unsigned portBASE_TYPE uxPriority )
{
	vSemaphoreCreateBinary(xLcdSemphr);

	/* Spawn the console task. */
	xTaskCreate( vLcdTask, ( signed char * ) "Lcd", lcdSTACK_SIZE, NULL, uxPriority, ( xTaskHandle * ) NULL );
}


static portTASK_FUNCTION( vLcdTask, pvParameters )
{
	unsigned int pressure;
	unsigned int xPos;
	unsigned int yPos;
	int i = 0;
	int j = 0;
	
	ButtonController buttonController; 

	portTickType xLastWakeTime;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	printf("Touchscreen task running\r\n");

	/* Initialise LCD display */
	/* NOTE: We needed to delay calling lcd_init() until here because it uses
	 * xTaskDelay to implement a delay and, as a result, can only be called from
	 * a task */
	lcd_init();

	buttonController = initialiseButtons();

	lcd_fillScreen(MAROON);


	/* Infinite loop blocks waiting for a touch screen interrupt event from
	 * the queue. */
	for( ;; )
	{
		/* Clear TS interrupts (EINT3) */
		/* Reset and (re-)enable TS interrupts on EINT3 */
		EXTINT = 8;						/* Reset EINT3 */

		/* Enable TS interrupt vector (VIC) (vector 17) */
		VICIntEnable = 1 << 17;			/* Enable interrupts on vector 17 */

		/* Block on a queue waiting for an event from the TS interrupt handler */
		xSemaphoreTake(xLcdSemphr, portMAX_DELAY);
				
		/* Disable TS interrupt vector (VIC) (vector 17) */
		VICIntEnClr = 1 << 17;

		/* +++ This point in the code can be interpreted as a screen button push event +++ */\

		/* Measure next sleep interval from this point */
		xLastWakeTime = xTaskGetTickCount();

		/* Start polling the touchscreen pressure and position ( getTouch(...) ) */
		/* Keep polling until pressure == 0 */
		getTouch(&xPos, &yPos, &pressure);
		
		if((xPos >= buttonController.masterButton.xpos && xPos <= buttonController.masterButton.xpos+buttonController.masterButton.length) 
				&& (yPos >= buttonController.masterButton.ypos && yPos <= buttonController.masterButton.ypos+buttonController.masterButton.length)){
			if(buttonController.masterButton.buttonOn == 0){
				buttonController.masterButton.buttonOn = 1;
				buttonController.masterButton.buttonColor = GREEN;
				buttonController.masterButton.backgroundFontColor = GREEN;
				strcpy(buttonController.masterButton.text, "MASTER ON");
			}else{
				buttonController.masterButton.buttonOn = 0;
				buttonController.masterButton.buttonColor = RED;
				buttonController.masterButton.backgroundFontColor = RED;
				strcpy(buttonController.masterButton.text, "MASTER OFF");	
			}
		}
		

		for(;i<4;i++){
			if((xPos >= buttonController.dimmers[i].xpos && xPos <= buttonController.dimmers[i].xpos+buttonController.dimmers[i].length) 
					&& (yPos >= buttonController.dimmers[i].ypos && yPos <= buttonController.dimmers[i].ypos+buttonController.dimmers[i].length)){
				if(buttonController.dimmers[i].buttonOn == 0){
					buttonController.dimmers[i].buttonColor = GREEN;
					buttonController.dimmers[i].backgroundFontColor = GREEN;
					buttonController.dimmers[i].buttonOn = 1;
				}else{
					buttonController.dimmers[i].buttonColor = RED;
					buttonController.dimmers[i].backgroundFontColor = RED;
					buttonController.dimmers[i].buttonOn = 0;
				}
			}
		}
		i = 0;
		
		for(;j<2;j++){
			if((xPos >= buttonController.presets[j].xpos && xPos <= buttonController.presets[j].xpos+buttonController.presets[j].length) 
					&& (yPos >= buttonController.presets[j].ypos && yPos <= buttonController.presets[j].ypos+buttonController.presets[j].length)){
				if(buttonController.presets[j].buttonOn == 0){
					buttonController.presets[j].buttonColor = YELLOW;
					buttonController.presets[j].backgroundFontColor = YELLOW;
					buttonController.presets[j].buttonOn = 1;
				}else{
					buttonController.presets[j].buttonColor = CYAN;
					buttonController.presets[j].backgroundFontColor = CYAN;
					buttonController.presets[j].buttonOn = 0;
				}
			}
		}
		j = 0;
		
		
					
		setButtonController(buttonController);
		
		while (pressure > 0)
		{
			/* Get current pressure */
			getTouch(&xPos, &yPos, &pressure);
	
				
			/* Delay to give us a 25ms periodic TS pressure sample */
			vTaskDelayUntil( &xLastWakeTime, 25 );
		}		

		/* +++ This point in the code can be interpreted as a screen button release event +++ */
		
		lcd_fillScreen(MAROON);
		drawDefaultInterface();
		
	}
}
	

void vLCD_ISRHandler( void )
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Process the touchscreen interrupt */
	/* We would want to indicate to the task above that an event has occurred */
	xSemaphoreGiveFromISR(xLcdSemphr, &xHigherPriorityTaskWoken);

	EXTINT = 8;					/* Reset EINT3 */
	VICVectAddr = 0;			/* Clear VIC interrupt */

	/* Exit the ISR.  If a task was woken by either a character being received
	or transmitted then a context switch will occur. */
	portEXIT_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


