#ifndef COMMANDS_H
#define COMMANDS_H

typedef struct Command
{
		short lastState[4];	// from 0-3
		short masterSwitch;
		short brightnessLevels[4]; // from 0-3
		short presetArray[2];
} Command;

#endif /* COMMANDS_H */
