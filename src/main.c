#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#include "dbgcon.h"
#include "input.h"
#include "mapfile.h"
#include "sprite.h"
#include "video/video.h"
#include "video/v_mode13.h"
#include "video/v_mode_y.h"

typedef unsigned char byte;

video_drv_t *mydrv = &mode13_drv;

void interrupt (*oldPitFunction)(void);
int game_tics = 0;
int game_seconds = 0;
byte collide_flag = 0;

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

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            mydrv->draw24(0, 8*row + col, col, row);
        }
    }
}

void displayMap(void) {
    mydrv->clear();
    mydrv->draw_plane(&current_room[0], 0);
    mydrv->draw_plane(&current_room[1], 0);
    mydrv->draw_plane(&current_room[2], 0);
    mydrv->draw_plane(&current_room[3], 0);
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
    byte used_vram;

    readGGS("data/elek1.ggs", 0);
    readGGS("data/elek2.ggs", 1);
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

    // needs to happen after the video init so VRAM is ready.
    mydrv->load_sprites(getSpriteSlot(0), 0, &used_vram);
    mydrv->load_sprites(getSpriteSlot(1), 1, &used_vram);

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

        if (collide_flag)
            mydrv->dbg_draw_solid(collision);

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

        // C key
        if (keyDown[0x2e])
            collide_flag ^= 1;

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
