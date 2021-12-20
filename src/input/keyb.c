#include "common/types.h"
#include "input/keyb.h"

#include <dos.h>
#include <stdlib.h>

bool input_setup = false;
void interrupt (*OldKeybIntVec)(void);

byte keyDown[101];
byte keyHeld[101];

static void interrupt far MyKbService(void) {
    unsigned char keycode, keytemp, keyval;
    keycode = inportb(0x60);
    //keys[keycode & 0x7f] = (keycode >> 7) ^ 1;
    keyval = (keycode >> 7) ^ 1;
    keyDown[keycode & 0x7f] = keyval;
    keyHeld[keycode & 0x7f] = keyval;
    
    keytemp = inportb(0x61);
    outportb(0x61, keytemp | 0x80);
    outportb(0x61, keytemp & 0x7f);

    outportb(0x20, 0x20);
}

void Input_Setup(void) {
    OldKeybIntVec = getvect(9);
    setvect(9, &MyKbService);
    atexit(Input_Shutdown);
    input_setup = true;
}

void Input_Shutdown(void) {
    if (!input_setup)
        return;

    setvect(9, OldKeybIntVec);
}