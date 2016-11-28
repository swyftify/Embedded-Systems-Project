#ifndef UI_H
#define UI_H
		
typedef struct Button{
	unsigned int xpos;
	unsigned int ypos;
	unsigned int length;
	unsigned short textXpos;
	unsigned short textYpos;
	unsigned char text[20];
	unsigned short fontColor;
	unsigned short backgroundFontColor;
	unsigned short buttonColor;
	unsigned short buttonOn;
} Button;		


typedef struct ButtonController{
	Button masterButton;
	Button * dimmers;
	Button * presets;
	Button * adjusters;
	Button * adjusterReading;
}ButtonController;
		
void drawDefaultInterface(void);

ButtonController initialiseButtons(void);

void setButtonController(ButtonController newController);

#endif