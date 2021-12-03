#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#include "dbgcon.h"
#include "input.h"
#include "mapfile.h"
#include "sprite.h"
#include "video.h"
#include "v_mode13.h"

typedef unsigned char byte;

video_drv_t *mydrv = &mode13_drv;

void interrupt (*oldPitFunction)(void);
int game_tics = 0;
int game_seconds = 0;

void load_pal(void) {
    color_t pal[256];
    FILE *f = fopen("data/em.pal", "rb");
    fread(&pal, 3, 256, f);
    fclose(f);

    mydrv->update_palette(pal);
}

void displaySheet(void) {
    int row;
    int col;
    byte *spr;
    byte **sheet;

    sheet = getSpriteSlot(0);
    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            spr = sheet[8 * row + col];
            if (!spr)
                continue;

            mydrv->draw24(spr, col, row);
        }
    }
}

void displayPlane(int plane) {
    int row;
    int col;
    int which;
    byte val;
    byte *spr;
    byte **sheets[2];

    sheets[0] = getSpriteSlot(0);
    sheets[1] = getSpriteSlot(1);

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 13; col++) {
            val = current_room[plane][row][col];
            if (!val)
                continue;

            which = (val & 64) ? 1 : 0;
            spr = sheets[which][val & 63];

            mydrv->draw24(spr, col, row);
            //drawSprite(spr, col, row);
        }
    }
}

void displayMap(void) {
    mydrv->clear();
    displayPlane(0);
    displayPlane(1);
    displayPlane(2);
    displayPlane(3);
}

void textMap(void) {
    int row;
    int col;
    byte val;

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 13; col++) {
            val = current_room[0][row][col];
            if (val)
                printf("%02x ", val);
            else
                printf(".. ");
        }
        printf("\n");
    }
}

void interrupt myPitTimer(void) {
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

int main() {
    int changed = 1;
    int cur_screen = 0;
    char buf[20];

    readGGC("data/elek1.ggs", 0);
    readGGC("data/elek2.ggs", 1);
    map_load("data/elek.ggc");
    /*
    textMap();

    while (!kbhit());
    getch();
    */

    PIT_Setup();
    Input_Setup();
    mydrv->init();
    load_pal();
    DbgCon_Init("data/BALD8X8.FNT", mydrv);
    //displaySheet();

    while (1) {
        if (keyHeld[0x1] || keyHeld[0x10])
            break;

        if (changed) {
            map_decode(cur_screen);
            sprintf(buf, "screen %02x", cur_screen);
            DbgCon_Insert(buf);
            changed = 0;
        }

        displayMap();
        DbgCon_Tick();
        DbgCon_Draw(game_seconds);

        if (keyDown[0x4b]) {
            // left
            cur_screen = (cur_screen & 240) | ((cur_screen - 1) & 15);
            changed = 1;
        } else if (keyDown[0x4d]) {
            // right
            cur_screen = (cur_screen & 240) | ((cur_screen + 1) & 15);
            changed = 1;
        } else if (keyDown[0x48]) {
            // up
            cur_screen = (cur_screen - 16) & 255;
            changed = 1;
        } else if (keyDown[0x50]) {
            // down
            cur_screen = (cur_screen + 16) & 255;
            changed = 1;
        }

        memset(keyDown, 0, 101);
        mydrv->update();
    }

    Input_Shutdown();
    mydrv->shutdown();
    map_free();
    free_sprites();
    DbgCon_Close();
    PIT_Close();

    return 0;
}
