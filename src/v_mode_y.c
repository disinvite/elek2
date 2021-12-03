#include "v_mode_y.h"

#include <dos.h>
#include <mem.h>

#include "video.h"

static byte *VGA = (byte*)(0xa0000000l);

// vid_ofs points to the *inactive* video page.
static int vid_ofs = 0x4000;

void init(void) {
    asm mov ax, 0x13
    asm int 0x10

    // turn off chain-4 mode
    outportb(SC_INDEX, SC_MEMORY_MODE);
    outportb(SC_DATA, 0x06);

    // turn off odd/even and set write mode 0
    outportb(GC_INDEX, GC_MODE);
    outportb(GC_DATA, 0x40);

    // disable chain
    outportb(GC_INDEX, GC_MISC);
    outportb(GC_DATA, 5);

    // clear screen
    outportb(SC_INDEX, SC_MAP_MASK);
    outportb(SC_DATA, 0xff);
    memset(VGA, 0, 16000);

    // turn off long mode
    outportb(CRTC_INDEX, CRTC_UNDERLINE);
    outportb(CRTC_DATA, 0x00);
    
    // turn on byte mode
    outportb(CRTC_INDEX, CRTC_MODE);
    outportb(CRTC_DATA, 0xe3);
}

void shutdown(void) {
    asm mov ax, 3
    asm int 0x10
}

void clear(void) {
    int plane;
    for (plane = 0; plane < 4; plane++) {
        // select the map mask register
        outportb(SC_INDEX, SC_MAP_MASK);

        // write 2^plane 
        outportb(SC_DATA, 1 << plane);
        
        memset(VGA + vid_ofs, 0, 16000);
    }
}

void update(void) {
    // Set CRTC start address high
    outport(0x3d4, 0xc | vid_ofs);
    vid_ofs = 0x4000 - vid_ofs;

    // wait for vblank
    while((inportb(0x3da) & 8) != 0);
    while((inportb(0x3da) & 8) == 0);
}

void set_fontface(byte *font) {
    // not yet
}

void type_msg(char *msg, int x, int y, byte color) {
    // not yet
}

void update_palette(color_t *pal) {
    int i;
    outportb(0x3c8, 0);

    for(i = 0; i < 256; i++) {
        outportb(0x3c9, pal[i].r);
        outportb(0x3c9, pal[i].g);
        outportb(0x3c9, pal[i].b);
    }
}

void draw24(byte *src, int x, int y) {
    int i, j;
    int plane;
    // x is in 24x24 block coords. we need to divide by 4
    // to deplane the pixel coord.
    // this results in a multiply by 6.
    int bx = x * 6;
    byte *start = VGA + vid_ofs + 80 * 24 * y + bx;

    for (plane = 0; plane < 4; plane++) {
        // select the map mask register
        outportb(SC_INDEX, SC_MAP_MASK);

        // write 2^plane 
        outportb(SC_DATA, 1 << plane);

        // 24 divided by 4 = 6
        for (i = 0; i < 6; i++) {
            for (j = 0; j < 24; j++) {
                start[80 * j + i] = src[24 * j + 4*i + plane];
            }
        }
    }
}

video_drv_t mode_y_drv = {
    &init,
    &shutdown,
    &clear,
    &update,
    &set_fontface,
    &type_msg,
    &update_palette,
    &draw24
};

