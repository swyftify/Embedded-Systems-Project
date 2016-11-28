#include "lcd_grph.h"
#include <stdio.h>
#include <stdlib.h>
#include "UI.h"


ButtonController buttonController;
Button presets[2];
Button dimmers[4];
Button adjusters[8];
Button adjusterReading[4];

ButtonController initialiseButtons(){
	Button masterButtonOn = {20,20,65,25,50,"MASTER OFF",BLACK,RED,RED,0};
	
	Button dimmerOne = {20,90,50,40,110,"D1",BLACK,RED,RED,0};
	Button dimmerTwo = {20,145,50,40,165,"D2",BLACK,RED,RED,0};
	Button dimmerThree = {20,200,50,40,225,"D3",BLACK,RED,RED,0};
	Button dimmerFour = {20,255,50,40,275,"D4",BLACK,RED,RED,0};
	
	Button presetOne = {90, 20, 65, 100, 50, "PRESET 1", BLACK, CYAN,CYAN,0};
	Button presetTwo = {160, 20, 65, 170, 50, "PRESET 2", BLACK, CYAN, CYAN, 0};
	
	Button adjusterDimmerOnePlus = {75,90,50,95,110,"+",WHITE,OLIVE,OLIVE};
	Button adjusterDimmerOneMinus = {130,90,50,150,110,"-",WHITE,OLIVE,OLIVE};
	Button adjusterDimmerTwoPlus = {75,145,50,95,165,"+",WHITE,OLIVE,OLIVE};
	Button adjusterDimmerTwoMinus = {130,145,50,150,165,"-",WHITE,OLIVE,OLIVE};
	Button adjusterDimmerThreePlus = {75,200,50,95,225,"+",WHITE,OLIVE,OLIVE};
	Button adjusterDimmerThreeMinus = {130,200,50,150,225,"-",WHITE,OLIVE,OLIVE};
	Button adjusterDimmerFourPlus = {75,255,50,95,275,"+",WHITE,OLIVE,OLIVE};
	Button adjusterDimmerFourMinus = {130,255,50,150,275,"-",WHITE,OLIVE,OLIVE};
	
	Button adjusterReadingOne = {185,90,50,205,110,"0",WHITE,BLACK,BLACK};
	Button adjusterReadingTwo = {185,145,50,205,165,"0",WHITE,BLACK,BLACK};
	Button adjusterReadingThree = {185,200,50,205,225,"0",WHITE,BLACK,BLACK};
	Button adjusterReadingFour = {185,255,50,205,275,"0",WHITE,BLACK,BLACK};
	
	adjusters[0] = adjusterDimmerOnePlus;
	adjusters[1] = adjusterDimmerOneMinus;
	adjusters[2] = adjusterDimmerTwoPlus;
	adjusters[3] = adjusterDimmerTwoMinus;
	adjusters[4] = adjusterDimmerThreePlus;
	adjusters[5] = adjusterDimmerThreeMinus;
	adjusters[6] = adjusterDimmerFourPlus;
	adjusters[7] = adjusterDimmerFourMinus;
	
	adjusterReading[0] = adjusterReadingOne;
	adjusterReading[1] = adjusterReadingTwo;
	adjusterReading[2] = adjusterReadingThree;
	adjusterReading[3] = adjusterReadingFour;
	
	buttonController.masterButton = masterButtonOn;
	
	dimmers[0] = dimmerOne;
	dimmers[1] = dimmerTwo;
	dimmers[2] = dimmerThree;
	dimmers[3] = dimmerFour;
		
	
	presets[0] = presetOne;
	presets[1] = presetTwo;
	
	buttonController.adjusterReading = adjusterReading;
	buttonController.adjusters = adjusters;
	buttonController.dimmers=dimmers;
	buttonController.presets = presets;
	
	return buttonController;
}

void drawDefaultInterface(){
		Button masterButtonOn = buttonController.masterButton;
		int i = 0;
		
		lcd_fillRect(masterButtonOn.xpos,masterButtonOn.ypos,masterButtonOn.xpos+masterButtonOn.length,masterButtonOn.ypos+masterButtonOn.length,masterButtonOn.buttonColor);
		lcd_fontColor(masterButtonOn.fontColor, masterButtonOn.backgroundFontColor);
		lcd_putString(masterButtonOn.textXpos,masterButtonOn.textYpos,masterButtonOn.text);
	
		for(;i<4;i++){
			lcd_fillRect(buttonController.dimmers[i].xpos,buttonController.dimmers[i].ypos,buttonController.dimmers[i].xpos+buttonController.dimmers[i].length,
				buttonController.dimmers[i].ypos+buttonController.dimmers[i].length,buttonController.dimmers[i].buttonColor);
			lcd_fontColor(buttonController.dimmers[i].fontColor, buttonController.dimmers[i].backgroundFontColor);
			lcd_putString(buttonController.dimmers[i].textXpos,buttonController.dimmers[i].textYpos,buttonController.dimmers[i].text);
		}
		i = 0;
		
		for(;i<2;i++){
			lcd_fillRect(buttonController.presets[i].xpos,buttonController.presets[i].ypos,buttonController.presets[i].xpos+buttonController.presets[i].length,
				buttonController.presets[i].ypos+buttonController.presets[i].length,buttonController.presets[i].buttonColor);
			lcd_fontColor(buttonController.presets[i].fontColor, buttonController.presets[i].backgroundFontColor);
			lcd_putString(buttonController.presets[i].textXpos,buttonController.presets[i].textYpos,buttonController.presets[i].text);
		}
		i = 0;
		
		for(;i<8;i++){
			lcd_fillRect(buttonController.adjusters[i].xpos,buttonController.adjusters[i].ypos,buttonController.adjusters[i].xpos+buttonController.adjusters[i].length,
				buttonController.adjusters[i].ypos+buttonController.adjusters[i].length,buttonController.adjusters[i].buttonColor);
			lcd_fontColor(buttonController.adjusters[i].fontColor, buttonController.adjusters[i].backgroundFontColor);
			lcd_putString(buttonController.adjusters[i].textXpos,buttonController.adjusters[i].textYpos,buttonController.adjusters[i].text);
		}
		i = 0;
		for(;i<4;i++){
			lcd_fillRect(buttonController.adjusterReading[i].xpos,buttonController.adjusterReading[i].ypos,buttonController.adjusterReading[i].xpos+buttonController.adjusterReading[i].length,
				buttonController.adjusterReading[i].ypos+buttonController.adjusterReading[i].length,buttonController.adjusterReading[i].buttonColor);
			lcd_fontColor(buttonController.adjusterReading[i].fontColor, buttonController.adjusterReading[i].backgroundFontColor);
			lcd_putString(buttonController.adjusterReading[i].textXpos,buttonController.adjusterReading[i].textYpos,buttonController.adjusterReading[i].text);
		}
		
}

void setButtonController(ButtonController newController){
	buttonController = newController;
}





