#include "common/types.h"
#include "input/timer.h"

#include <dos.h>

static void interrupt (*oldPitFunction)(void);
int game_tics = 0;
int game_seconds = 0;

static void interrupt myPitTimer(void) {
    game_tics++;
    if (game_tics >= 8523) {
        game_tics = 0;
        game_seconds++;
    }

    outportb(0x20, 0x20);
}

void PIT_Setup(void) {
    oldPitFunction = getvect(8);
    setvect(8, &myPitTimer);

    // PIT = ~8523hz
    outportb(0x43, 0x34);
    outportb(0x40, 0x8c);
    outportb(0x40, 0);
}

void PIT_Close(void) {
    setvect(8, oldPitFunction);
    outportb(0x43, 0x34);
    outportb(0x40, 0xff);
    outportb(0x40, 0xff);
}
