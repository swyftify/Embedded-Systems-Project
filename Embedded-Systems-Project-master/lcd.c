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
#include "sensors.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "commands.h"

/* Maximum task stack size */
#define lcdSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )
	
void drawDefaultUI();

/* Interrupt handlers */
extern void vLCD_ISREntry( void );
void vLCD_ISRHandler( void );
void vTimerCallback1(TimerHandle_t xTimer);

/* The LCD task. */
static void vLcdTask( void *pvParameters );

/* Semaphore for ISR/task synchronisation */
xSemaphoreHandle xLcdSemphr;

xQueueHandle localQueue;

TimerHandle_t timer;

int x0 = 0;
int y0 = 313;
int x1 = 0;
int y1 = 315;


void vStartLcd( unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue)
{
	static xQueueHandle xCmdQ;
	
	vSemaphoreCreateBinary(xLcdSemphr);

	localQueue = xQueue;
	/* Spawn the console task. */
	xTaskCreate( vLcdTask, ( signed char * ) "Lcd", lcdSTACK_SIZE, &localQueue, uxPriority, ( xTaskHandle * ) NULL );
	
	timer = xTimerCreate("Main Light Timer" ,500, pdFALSE, ( void * ) 0, vTimerCallback1 );
}

static ButtonController buttonController; 

// Increment flag 1 if ++, 0 if --
Command limiter(Command cmd, int incrementFlag, int lightIndex){
	if(incrementFlag == 1 && cmd.brightnessLevels[lightIndex] == 3){
		cmd.brightnessLevels[lightIndex] = 3;
	}else if(incrementFlag == 0 && cmd.brightnessLevels[lightIndex] == 0){
		cmd.brightnessLevels[lightIndex] = 0;
	}else if (incrementFlag == 1){
		cmd.brightnessLevels[lightIndex]++; 
	}else if(incrementFlag == 0){
		cmd.brightnessLevels[lightIndex]--;
	}
	return cmd;
}

void vTimerCallback1(TimerHandle_t xTimer){
	xTimerStop( xTimer, 0 );
}

void loading(){
	while(xTimerIsTimerActive(timer)){
		lcd_fillRect(x0,y0,x1,y1, BLUE);
		lcd_fontColor(WHITE, BLACK);
		lcd_putString(80,307,"LOADING CHANGES");
		x1+=2;
	}
	x1 = 0;
}

static portTASK_FUNCTION( vLcdTask, pvParameters )
{
	unsigned int pressure;
	unsigned int xPos;
	unsigned int yPos;
	int i = 0;
	int j = 0;
	int total = 0;
	int activePreset = 0;

	Command cmd;
	Command cmdToInterface;

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

	lcd_fillScreen(BLACK);

	for(;i<4;i++){
		cmd.brightnessLevels[i] = 0;
		}
	i = 0;
	
	/* Infinite loop blocks waiting for a touch screen interrupt event from
	 * the queue. */
	for( ;; )
	{
		lcd_putString(25,10,"When Lights Go Off Tap To Refresh");
		
		buttonController = getButtonController();
		printf("lcd controller %u \r\n", buttonController.masterButton.buttonOn);
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
				for(;i < 4;i++){
					cmd.brightnessLevels[i] = 3;
				}
				cmd.masterSwitch = 1;
			}else{
				buttonController.masterButton.buttonOn = 0;
				buttonController.masterButton.buttonColor = RED;
				buttonController.masterButton.backgroundFontColor = RED;
				strcpy(buttonController.masterButton.text, "MASTER OFF");	
				for(;i < 4;i++){
					cmd.brightnessLevels[i] = 0;
				}
				cmd.masterSwitch = 0;
				
			}
		}
		

		for(;i<4;i++){
			if((xPos >= buttonController.dimmers[i].xpos && xPos <= buttonController.dimmers[i].xpos+buttonController.dimmers[i].length) 
					&& (yPos >= buttonController.dimmers[i].ypos && yPos <= buttonController.dimmers[i].ypos+buttonController.dimmers[i].length)){
				if(buttonController.dimmers[i].buttonOn == 0){
					buttonController.dimmers[i].buttonColor = GREEN;
					buttonController.dimmers[i].backgroundFontColor = GREEN;
					buttonController.dimmers[i].buttonOn = 1;
					cmd.brightnessLevels[i] = 3;
					buttonController.adjusterReading[i].buttonColor = WHITE;
					buttonController.adjusterReading[i].lengthx = 40;
					total++;
				}else{
					buttonController.dimmers[i].buttonColor = LIGHT_GRAY;
					buttonController.dimmers[i].backgroundFontColor = LIGHT_GRAY;
					buttonController.dimmers[i].buttonOn = 0;
					cmd.brightnessLevels[i] = 0;
					buttonController.adjusterReading[i].buttonColor = BLACK;
					strcpy(buttonController.adjusterReading[i].text, "OFF");
					total--;
				}
			}
		}
		i = 0;
		
		for(;j<2;j++){
			if((xPos >= buttonController.presets[j].xpos && xPos <= buttonController.presets[j].xpos+buttonController.presets[j].length) 
					&& (yPos >= buttonController.presets[j].ypos && yPos <= buttonController.presets[j].ypos+buttonController.presets[j].length)){
				if(buttonController.presets[j].buttonOn == 0 && activePreset == 0){
					activePreset++;
					buttonController.presets[j].buttonColor = YELLOW;
					buttonController.presets[j].backgroundFontColor = YELLOW;
					buttonController.presets[j].buttonOn = 1;
					cmd.presetArray[j] = 1;
						switch(j){
							case 0:
								cmd.brightnessLevels[0] = 1;
								cmd.brightnessLevels[1] = 1;
								cmd.brightnessLevels[2] = 1;
								cmd.brightnessLevels[3] = 1;
								break;
							case 1:
								cmd.brightnessLevels[0] = 1;
								cmd.brightnessLevels[1] = 2;
								cmd.brightnessLevels[2] = 3;
								cmd.brightnessLevels[3] = 3;
							break;
							default:
								break;
					}
				}else if(buttonController.presets[j].buttonOn == 1 && activePreset == 1){
					activePreset--;
					buttonController.presets[j].buttonColor = CYAN;
					buttonController.presets[j].backgroundFontColor = CYAN;
					buttonController.presets[j].buttonOn = 0;
					cmd.presetArray[j] = 0;
					for(i = 0;i < 4;i++){
						cmd.brightnessLevels[i] = 0;
					}
				}
			}
		}
		j = 0;
		i = 0;
		
		for(;i<8;i++){
			if((xPos >= buttonController.adjusters[i].xpos && xPos <= buttonController.adjusters[i].xpos+buttonController.adjusters[i].length) 
					&& (yPos >= buttonController.adjusters[i].ypos && yPos <= buttonController.adjusters[i].ypos+buttonController.adjusters[i].length)){
						switch(i){
							case 0:
								cmd = limiter(cmd, 1, 0);
								break;
							case 1:
								cmd = limiter(cmd, 0, 0);
								break;
							case 2:
								cmd = limiter(cmd, 1, 1);
								break;
							case 3:
								cmd = limiter(cmd, 0, 1);
								break;
							case 4:
								cmd = limiter(cmd, 1, 2);
								break;
							case 5:
								cmd = limiter(cmd, 0, 2);
								break;
							case 6:
								cmd = limiter(cmd, 1, 3);
								break;
							case 7:
								cmd = limiter(cmd, 0, 3);
								break;
							default:
								break;
						}
			}
		}
		i = 0;
		for(;i< 4;i++){
			if(cmd.brightnessLevels[i] != 0){
				buttonController.dimmers[i].buttonColor = GREEN;
				buttonController.dimmers[i].backgroundFontColor = GREEN;
				buttonController.dimmers[i].buttonOn = 1;
			}else{
				buttonController.dimmers[i].buttonColor = LIGHT_GRAY;
				buttonController.dimmers[i].backgroundFontColor = LIGHT_GRAY;
				buttonController.dimmers[i].buttonOn = 0;
			}
		}
		i = 0;
		for(;i<4;i++){
			switch(cmd.brightnessLevels[i]){
				case 0:
					buttonController.adjusterReading[i].buttonColor = BLACK;
					buttonController.adjusterReading[i].lengthx = 13;
					strcpy(buttonController.adjusterReading[i].text, "OFF");
					break;
				case 1:
					buttonController.adjusterReading[i].buttonColor = GREEN;
					strcpy(buttonController.adjusterReading[i].text, "");
					buttonController.adjusterReading[i].lengthx = 13;
					break;
				case 2:
					buttonController.adjusterReading[i].buttonColor = YELLOW;
					strcpy(buttonController.adjusterReading[i].text, "");
					buttonController.adjusterReading[i].lengthx = 27;
					break;
				case 3:
					buttonController.adjusterReading[i].buttonColor = WHITE;
					strcpy(buttonController.adjusterReading[i].text, "");
					buttonController.adjusterReading[i].lengthx = 40;
					break;
				default:
					break;
			}
		}
		i = 0;
					
		setButtonController(buttonController);
		//lastState = currentState;
		
		xTimerStart(timer, 0);
		loading();
		
		while (pressure > 0)
		{
			/* Get current pressure */
			getTouch(&xPos, &yPos, &pressure);
	
				
			/* Delay to give us a 25ms periodic TS pressure sample */
			vTaskDelayUntil( &xLastWakeTime, 25 );
		}		

		/* +++ This point in the code can be interpreted as a screen button release event +++ */
		
		lcd_fillScreen(BLACK);
		drawDefaultInterface();
		

		xQueueSendToBack(localQueue, &cmd, 0);
		printf("Command %u", cmd.masterSwitch);
		printf("SENT FROM LCD\r\n");
		
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


