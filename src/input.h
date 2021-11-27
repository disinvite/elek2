#ifndef INPUT_H
#define INPUT_H

typedef unsigned char byte;

extern byte keyDown[101];
extern byte keyHeld[101];


void Input_Setup(void);
void Input_Shutdown(void);

#endif
