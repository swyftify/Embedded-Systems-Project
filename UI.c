#include "lcd_grph.h"
#include <stdio.h>
#include <stdlib.h>
#include "UI.h"


ButtonController buttonController;

ButtonController initialiseButtons(){
	//Button * dimmers;
	Button masterButtonOn = {20,20,65,25,50,"MASTER OFF",BLACK,RED,RED,0};
	
	Button dimmerOne = {20,90,50,40,110,"D1",BLACK,RED,RED,0};
	Button dimmerTwo = {20,145,50,40,165,"D2",BLACK,RED,RED,0};
	Button dimmerThree = {20,200,50,40,225,"D3",BLACK,RED,RED,0};
	Button dimmerFour = {20,255,50,40,275,"D4",BLACK,RED,RED,0};
	
	buttonController.masterButton = masterButtonOn;
	
	//dimmers[0] = dimmerOne;
	buttonController.dimmerButtonOne = dimmerOne;
	buttonController.dimmerButtonTwo = dimmerTwo;
	buttonController.dimmerButtonThree = dimmerThree;
	buttonController.dimmerButtonFour = dimmerFour;
	
	return buttonController;
}

void drawDefaultInterface(){
		Button masterButtonOn = buttonController.masterButton;
		Button dimmerOne = buttonController.dimmerButtonOne;
		Button dimmerTwo = buttonController.dimmerButtonTwo;
		Button dimmerThree = buttonController.dimmerButtonThree;
		Button dimmerFour = buttonController.dimmerButtonFour;
	
		lcd_fillRect(masterButtonOn.xpos,masterButtonOn.ypos,masterButtonOn.xpos+masterButtonOn.length,masterButtonOn.ypos+masterButtonOn.length,masterButtonOn.buttonColor);
		lcd_fontColor(masterButtonOn.fontColor, masterButtonOn.backgroundFontColor);
		lcd_putString(masterButtonOn.textXpos,masterButtonOn.textYpos,masterButtonOn.text);
	
		lcd_fillRect(dimmerOne.xpos,dimmerOne.ypos,dimmerOne.xpos+dimmerOne.length,dimmerOne.ypos+dimmerOne.length,dimmerOne.buttonColor);
		lcd_fontColor(dimmerOne.fontColor, dimmerOne.backgroundFontColor);
		lcd_putString(dimmerOne.textXpos,dimmerOne.textYpos,dimmerOne.text);
	
		lcd_fillRect(dimmerTwo.xpos,dimmerTwo.ypos,dimmerTwo.xpos+dimmerTwo.length,dimmerTwo.ypos+dimmerTwo.length,dimmerTwo.buttonColor);
		lcd_fontColor(dimmerTwo.fontColor, dimmerTwo.backgroundFontColor);
		lcd_putString(dimmerTwo.textXpos,dimmerTwo.textYpos,dimmerTwo.text);
	
		lcd_fillRect(dimmerThree.xpos,dimmerThree.ypos,dimmerThree.xpos+dimmerThree.length,dimmerThree.ypos+dimmerThree.length,dimmerThree.buttonColor);
		lcd_fontColor(dimmerThree.fontColor, dimmerThree.backgroundFontColor);
		lcd_putString(dimmerThree.textXpos,dimmerThree.textYpos,dimmerThree.text);
		
		lcd_fillRect(dimmerFour.xpos,dimmerFour.ypos,dimmerFour.xpos+dimmerFour.length,dimmerFour.ypos+dimmerFour.length,dimmerFour.buttonColor);
		lcd_fontColor(dimmerFour.fontColor, dimmerFour.backgroundFontColor);
		lcd_putString(dimmerFour.textXpos,dimmerFour.textYpos,dimmerFour.text);
		
		
}

void setButtonController(ButtonController newController){
	buttonController = newController;
}





