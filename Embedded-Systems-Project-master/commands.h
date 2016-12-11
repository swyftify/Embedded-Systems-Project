#ifndef COMMANDS_H
#define COMMANDS_H

typedef struct Command
{
		int masterSwitch;
		int brightnessLevels[4]; // from 0-3
		int presetArray[2];
} Command;

#endif /* COMMANDS_H */
