#include <conio.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#include "mapfile.h"
#include "sprite.h"

typedef unsigned char byte;

byte *VGA = (byte*)(0xa0000000l);

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
    for (i = 0; i < 24; i++) {
        //memcpy(&VGA[start + 320*i], &sprite[24*i], 24);
        for (j = 0; j < 24; j++) {
            if (sprite[24*i + j])
                VGA[start + 320*i + j] = sprite[24*i + j];
        }
    }
}

void video_start(void) {
    asm mov ax, 0x13
    asm int 0x10

    load_pal();

    memset(VGA, 0, 64000);
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
    readGGC("data/elek1.ggs", 0);
    readGGC("data/elek2.ggs", 1);
    map_load("data/elek.ggc");
    map_decode(1);
    /*
    textMap();

    while (!kbhit());
    getch();
    */

    video_start();
    //displaySheet();
    displayMap();

    while (!kbhit());
    getch();

    restore_video();
    map_free();
    free_sprites();
    return 0;
}
