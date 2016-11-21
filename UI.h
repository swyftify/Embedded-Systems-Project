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
	Button dimmerButtonOne;
	Button dimmerButtonTwo;
	Button dimmerButtonThree;
	Button dimmerButtonFour;
}ButtonController;
		
void drawDefaultInterface();

ButtonController initialiseButtons();

void setButtonController(ButtonController newController);



#endif