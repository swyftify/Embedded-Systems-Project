#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "lpc24xx.h"
#include <stdio.h>
#include <string.h>
#include "sensors.h"
#include "commands.h"
#include "lcd.h"

#define I2C_AA      0x00000004 		// Frequence Prescaler 1?
#define I2C_SI      0x00000008		// LS2 ?
#define I2C_STO     0x00000010		
#define I2C_STA     0x00000020
#define I2C_I2EN    0x00000040

#define lightsSTACK_SIZE			( ( unsigned portBASE_TYPE ) 256 )
	
static void vLightsTask( void *pvParameters );
void vTimerCallback( TimerHandle_t xTimer );
void brightness();

// Define the timer
TimerHandle_t lightTimer;

void vStartLights( unsigned portBASE_TYPE uxPriority, xQueueHandle xQueue)
{
	static xQueueHandle xCmdQ;
	/* Enable and configure I2C0 */
	PCONP    |=  (1 << 7);                /* Enable power for I2C0              */

	/* Initialize pins for SDA (P0.27) and SCL (P0.28) functions                */
	PINSEL1  &= ~0x03C00000;
	PINSEL1  |=  0x01400000;

	/* Clear I2C state machine                                                  */
	I20CONCLR =  I2C_AA | I2C_SI | I2C_STA | I2C_I2EN;
	
	/* Setup I2C clock speed                                                    */
	I20SCLL   =  0x80;
	I20SCLH   =  0x80;
	
	I20CONSET =  I2C_I2EN;
	
	//------------------------------------------------------------------------------------------------
	
  xCmdQ = xQueue;

	/* Spawn the console task . */
	xTaskCreate( vLightsTask, ( signed char * ) "Lights", lightsSTACK_SIZE, &xCmdQ, uxPriority, ( xTaskHandle * ) NULL );
	
	lightTimer = xTimerCreate("Main Light Timer" ,4000, pdFALSE, ( void * ) 0, vTimerCallback );

	printf("Lights task started with timer ...\r\n");
}

void getleds(unsigned int mask)
{
	/* Initialise */
	I20CONCLR =  I2C_AA | I2C_SI | I2C_STA | I2C_STO;
	
	/* Request send START */
	I20CONSET =  I2C_STA;

	/* Wait for START to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Request send PCA9532 ADDRESS and Read bit and clear SI */		
	I20DAT    =  0xC0;
	I20CONCLR =  I2C_SI | I2C_STA;

	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Send control word to read PCA9532 LS2 register */
	I20DAT = 0x8;
	I20CONCLR =  I2C_SI;

	/* Wait for DATA with control word to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Request send LED on data */		
	I20DAT    =  mask;
	I20CONCLR =  I2C_SI | I2C_STA;

	/* Wait for data to be sent */
	while (!(I20CONSET & I2C_SI));

	I20CONCLR = I2C_SI;
	
	/* Request send NAQ and STOP */
	I20CONSET =  I2C_STO;
	I20CONCLR =  I2C_SI | I2C_AA;

	/* Wait for STOP to be sent */
	while (I20CONSET & I2C_STO);
}

void brightness(unsigned int registerVal, unsigned int val)
{

	/* Initialise */
	I20CONCLR =  I2C_AA | I2C_SI | I2C_STA | I2C_STO;
	
	/* Request send START */
	I20CONSET =  I2C_STA;

	/* Wait for START to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Request send PCA9532 ADDRESS and Read bit and clear SI */		
	I20DAT    =  0xC0;
	I20CONCLR =  I2C_SI | I2C_STA;

	/* Wait for ADDRESS and R/W to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Send control word to read PCA9532 LS2 register */
	I20DAT = registerVal;
	I20CONCLR =  I2C_SI;

	/* Wait for DATA with control word to be sent */
	while (!(I20CONSET & I2C_SI));

	/* Request send LED on data */		
	I20DAT    =  val;
	I20CONCLR =  I2C_SI | I2C_STA;

	/* Wait for data to be sent */
	while (!(I20CONSET & I2C_SI));

	I20CONCLR = I2C_SI;

	/* Request send NAQ and STOP */
	I20CONSET =  I2C_STO;
	I20CONCLR =  I2C_SI | I2C_AA;

	/* Wait for STOP to be sent */
	while (I20CONSET & I2C_STO);
}

void vTimerCallback( TimerHandle_t xTimer ) {
	//Stop the timer when it expires
	xTimerStop( xTimer, 0 );
	getleds(0x00);
	
	printf("Timer Expired");
}


static portTASK_FUNCTION( vLightsTask, pvParameters )
{
	portTickType xLastWakeTime;
	unsigned char buttonState;
	unsigned char lastButtonState;
	unsigned char changedState;
	int i;
	unsigned char defaultMask;
	unsigned int brightnessValue;
	xQueueHandle xCmdQ;
  Command cmd;
	
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	
	xCmdQ = * ( ( xQueueHandle * ) pvParameters );
	
	/* pvParameters is actually a pointer to an xQueueHandle. Cast it and then
		 dereference it to save it for later use. */

	printf("Starting lighting poll ...\r\n");

	/* initialise lastState with all buttons off */
	lastButtonState = 0;

	/* initial xLastWakeTime for accurate polling interval */
	xLastWakeTime = xTaskGetTickCount();
	
	brightness(0x03,0x05);	// PWM0
	brightness(0x05,0x20);		// PWM1
					 
	while( 1 )
	{
			defaultMask = 0x00;
			xQueueReceive(xCmdQ, &cmd, portMAX_DELAY);
		
			if(cmd.masterSwitch == 1){
				xTimerStart(lightTimer, 0);
				defaultMask = 0x55; 
			}
			if(cmd.masterSwitch == 0){
				defaultMask = 0x00;
			}
			if(cmd.presetArray[1] == 1){
				brightness(0x02,0xC0);
				brightness(0x04,0x10);					
			}
			else if(cmd.presetArray[1] == 0){
				brightness(0x02,0x01);	
				brightness(0x04,0x01);		
			}
			
			for(i = 0;i<4;i++){
				xTimerStart(lightTimer, 0);
				switch(i){
					case 0:
						if(cmd.brightnessLevels[i]==1) defaultMask |= 0x2;
						if(cmd.brightnessLevels[i]==2) defaultMask |= 0x3;
						if(cmd.brightnessLevels[i]==3) defaultMask |= 0x1;
					break;
					case 1:
						if(cmd.brightnessLevels[i]==1) defaultMask |= 0x8;
						if(cmd.brightnessLevels[i]==2) defaultMask |= 0xC;
						if(cmd.brightnessLevels[i]==3) defaultMask |= 0x4;
					break;
					case 2:
						if(cmd.brightnessLevels[i]==1) defaultMask |= 0x20;
						if(cmd.brightnessLevels[i]==2) defaultMask |= 0x30;
						if(cmd.brightnessLevels[i]==3) defaultMask |= 0x10;
					break;
					case 3:
						if(cmd.brightnessLevels[i]==1) defaultMask |= 0x80;
						if(cmd.brightnessLevels[i]==2) defaultMask |= 0xC0;
						if(cmd.brightnessLevels[i]==3) defaultMask |= 0x40;
					break;
					default:
						break;
				}
			}
			i = 0;
			
			vTaskDelayUntil( &xLastWakeTime, 20);
			getleds(defaultMask);
    	vTaskDelayUntil( &xLastWakeTime, 20);
		
			
	}
}
