#include "lcd_grph.h"
#include <stdio.h>
#include <stdlib.h>
#include "UI.h"


ButtonController buttonController;

ButtonController initialiseButtons(){
	Button masterButtonOn = {20,20,65,25,50,"MASTER OFF",BLACK,RED,RED,0};
	
	Button dimmerOne = {20,90,50,40,110,"D1",BLACK,RED,RED,0};
	Button dimmerTwo = {20,145,50,40,165,"D2",BLACK,RED,RED,0};
	Button dimmerThree = {20,200,50,40,225,"D3",BLACK,RED,RED,0};
	Button dimmerFour = {20,255,50,40,275,"D4",BLACK,RED,RED,0};
	
	Button presetOne = {90, 20, 65, 100, 50, "PRESET1", BLACK, CYAN,CYAN,0};
	Button presetTwo = {160, 20, 65, 170, 50, "PRESET 2", BLACK, CYAN, CYAN, 0};
	
	buttonController.masterButton = masterButtonOn;
	
	buttonController.dimmers[0] = dimmerOne;
	buttonController.dimmers[1] = dimmerTwo;
	buttonController.dimmers[2] = dimmerThree;
	buttonController.dimmers[3] = dimmerFour;

	buttonController.presets[0] = presetOne;
	buttonController.presets[1] = presetTwo;
	
	return buttonController;
}

void drawDefaultInterface(){
		Button masterButtonOn = buttonController.masterButton;
		int i = 0;
		int j = 0;
	
		lcd_fillRect(masterButtonOn.xpos,masterButtonOn.ypos,masterButtonOn.xpos+masterButtonOn.length,masterButtonOn.ypos+masterButtonOn.length,masterButtonOn.buttonColor);
		lcd_fontColor(masterButtonOn.fontColor, masterButtonOn.backgroundFontColor);
		lcd_putString(masterButtonOn.textXpos,masterButtonOn.textYpos,masterButtonOn.text);
	
		for(;i<4;i++){
			lcd_fillRect(buttonController.dimmers[i].xpos,buttonController.dimmers[i].ypos,buttonController.dimmers[i].xpos+buttonController.dimmers[i].length,
				buttonController.dimmers[i].ypos+buttonController.dimmers[i].length,buttonController.dimmers[i].buttonColor);
			lcd_fontColor(buttonController.dimmers[i].fontColor, buttonController.dimmers[i].backgroundFontColor);
			lcd_putString(buttonController.dimmers[i].textXpos,buttonController.dimmers[i].textYpos,buttonController.dimmers[i].text);
		}
		
		for(;j<2;j++){
			lcd_fillRect(buttonController.presets[j].xpos,buttonController.presets[j].ypos,buttonController.presets[j].xpos+buttonController.presets[j].length,
				buttonController.presets[j].ypos+buttonController.presets[j].length,buttonController.presets[j].buttonColor);
			lcd_fontColor(buttonController.presets[j].fontColor, buttonController.presets[j].backgroundFontColor);
			lcd_putString(buttonController.presets[j].textXpos,buttonController.presets[j].textYpos,buttonController.presets[j].text);
		}
		
}

void setButtonController(ButtonController newController){
	buttonController = newController;
}





