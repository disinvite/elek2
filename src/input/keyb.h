#ifndef INPUT_KEYB_H
#define INPUT_KEYB_H

#include "common/types.h"

extern byte keyDown[101];
extern byte keyHeld[101];

int Keyb_PumpEvents(int *keycode);
void Input_Setup(void);
void Input_Shutdown(void);

#endif
