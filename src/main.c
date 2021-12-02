#include <conio.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#include "dbgcon.h"
#include "input.h"
#include "mapfile.h"
#include "sprite.h"

typedef unsigned char byte;

byte *VGA = (byte*)(0xa0000000l);
byte *offscreen;

void load_pal(void) {
    int i;
    color_t pal[256];
    FILE *f = fopen("data/em.pal", "rb");
    fread(&pal, 3, 256, f);
    fclose(f);

    outportb(0x3c8, 0);
    for(i = 0; i < 256; i++) {
        outportb(0x3c9, pal[i].r);
        outportb(0x3c9, pal[i].g);
        outportb(0x3c9, pal[i].b);
    }
}

void drawSprite(byte *sprite, int tx, int ty) {
    int i, j;
    int start = 320 * 24 * ty + 24 * tx;
    
    /*
    for (i = 0; i < 24; i++) {
        //memcpy(&VGA[start + 320*i], &sprite[24*i], 24);
        for (j = 0; j < 24; j++) {
            if (sprite[24*i + j])
                VGA[start + 320*i + j] = sprite[24*i + j];
        }
    }
    */

    asm push ds
    asm push es

    asm mov cx, 24
    asm les di, [offscreen]
    asm lds si, [sprite]
    asm add di, [start]
pixrow:
    asm mov dx, 24
pixcol:
    asm lodsb
    asm and al, al // 0 pixel is transparent.
    asm jnz skipload
    asm mov al, es:[di]
skipload:
    asm stosb
    asm dec dx
    asm jnz pixcol

    asm add di, 296
    asm dec cx
    asm jnz pixrow

    asm pop es
    asm pop ds
}

void video_start(void) {
    asm mov ax, 0x13
    asm int 0x10

    load_pal();

    memset(VGA, 0, 64000);
}

void WaitForVblank(void) {
    while((inportb(0x3da) & 8) != 0);
    while((inportb(0x3da) & 8) == 0);
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

            drawSprite(spr, col, row);
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

            drawSprite(spr, col, row);
        }
    }
}

void displayMap(void) {
    memset(offscreen, 0, 64000);
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

void restore_video(void) {
    asm mov ax, 3
    asm int 0x10
}

int main() {
    int changed = 1;
    int cur_screen = 0;
    char buf[20];

    offscreen = malloc(64000);

    readGGC("data/elek1.ggs", 0);
    readGGC("data/elek2.ggs", 1);
    map_load("data/elek.ggc");
    /*
    textMap();

    while (!kbhit());
    getch();
    */

    Input_Setup();
    video_start();
    DbgCon_Init("data/BALD8X8.FNT");
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
        DbgCon_Draw(offscreen);

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
        WaitForVblank();
        memcpy(VGA, offscreen, 64000);
    }

    Input_Shutdown();
    restore_video();
    map_free();
    free_sprites();
    DbgCon_Close();

    free(offscreen);

    return 0;
}
