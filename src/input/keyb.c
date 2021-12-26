#include "common/types.h"
#include "input/keyb.h"

#include <dos.h>
#include <stdlib.h>

bool input_setup = false;
void interrupt (*OldKeybIntVec)(void);

byte keyDown[101];
byte keyHeld[101];

#define kMaxKeybEvents 16

static int evt_buf[kMaxKeybEvents];
static int evt_write_ptr = 0;
static int evt_read_ptr  = 0;

static void interrupt far MyKbService(void) {
    unsigned char keycode, keytemp, keyval;
    keycode = inportb(0x60);
    //keys[keycode & 0x7f] = (keycode >> 7) ^ 1;
    keyval = (keycode >> 7) ^ 1;
    keyDown[keycode & 0x7f] = keyval;
    keyHeld[keycode & 0x7f] = keyval;
    
    evt_buf[evt_write_ptr] = keycode;
    evt_write_ptr++;
    evt_write_ptr &= (kMaxKeybEvents - 1);

    // Overflow detected
    if (evt_write_ptr == evt_read_ptr) {
        evt_read_ptr++;
        evt_read_ptr &= (kMaxKeybEvents - 1);
    }
    
    keytemp = inportb(0x61);
    outportb(0x61, keytemp | 0x80);
    outportb(0x61, keytemp & 0x7f);

    outportb(0x20, 0x20);
}

int Keyb_PumpEvents(int *keycode) {
    if (evt_read_ptr == evt_write_ptr)
        return 0;

    *keycode = evt_buf[evt_read_ptr];
    evt_read_ptr++;
    evt_read_ptr &= (kMaxKeybEvents - 1);
    return 1;
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