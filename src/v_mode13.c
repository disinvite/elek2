#include "v_mode13.h"

#include <dos.h>
#include <mem.h>
#include <stdlib.h>
#include <string.h>

#include "video.h"

typedef unsigned char byte;

#define kMaxVidSrc 8

static byte old_video_mode; // TODO: use this I guess

static byte *VGA = (byte*)(0xa0000000l);
static byte *offscreen;

// 8x8 pixel font for debug display
static byte *pixel_font = 0;

static byte *buffer_src[kMaxVidSrc][64];

static void init(void) {
    asm mov ax, 0x13
    asm int 0x10

    offscreen = malloc(64000);
}

static void shutdown(void) {
    asm mov ax, 3
    asm int 0x10

    free(offscreen);
}

static void clear(void) {
    memset(offscreen, 0, 64000);
}

static int load_sprites(byte **sprites, byte slot, byte *used_vram) {
    memcpy(buffer_src[slot], sprites, 256);
    *used_vram = 0;

    return 0;
}

static void update(void) {
    // wait for vblank
    while((inportb(0x3da) & 8) != 0);
    while((inportb(0x3da) & 8) == 0);

    memcpy(VGA, offscreen, 64000);
}

// just assume you got the full 256 colors for now.
static void update_palette(color_t *pal) {
    int i;
    outportb(0x3c8, 0);

    for(i = 0; i < 256; i++) {
        outportb(0x3c9, pal[i].r);
        outportb(0x3c9, pal[i].g);
        outportb(0x3c9, pal[i].b);
    }
}

static void set_fontface(byte *font) {
    pixel_font = font;
}

static void type_msg(char *msg, int x, int y, byte color) {
    int i;
    int row;
    byte b;
    int ofs;
    int t;
    int len;

    if (!pixel_font)
        return;

    //byte *start = &buf[320 * y];
    len = strlen(msg);

    for (i = 0; i < len; i++) {
        ofs = msg[i] & 127;
        for (row = 0; row < 8; row++) {
            b = pixel_font[8*ofs + row];
            for (t = 0; t < 8; t++) {
                if (b&128)
                    offscreen[320 * (y+row) + x + t] = color;
                    //*(start + 320*row + x + t) = color;
                b <<= 1;
            }
        }

        x += 8;
        // TODO: wrapping or whatever?
        if (x > 312)
            break;
    }
}

static void draw24(byte slot, byte id, int x, int y) {
    int i, j;
    int start;
    
    byte *src = buffer_src[slot][id];
    if (!src)
        return;

    start = 320 * 24 * y + 24 * x;

    asm push ds
    asm push es

    asm mov cx, 24
    asm les di, [offscreen]
    asm lds si, [src]
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

video_drv_t mode13_drv = {
    &init,
    &shutdown,
    &clear,
    &update,
    &load_sprites,
    &set_fontface,
    &type_msg,
    &update_palette,
    &draw24
};
