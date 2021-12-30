#include "common/types.h"
#include "video/v_mode13.h"

#include <dos.h>
#include <mem.h>
#include <stdlib.h>
#include <string.h>

#include "map/screen.h" // For access to `current_room`
#include "video/video.h"

#define kMaxVidSrc 8

static byte old_video_mode; // TODO: use this I guess

static byte *VGA = (byte*)(0xa0000000l);

// Use segment aligned pointer for offscreen buffer.
// This is so we can assume that ES:DI on offscreen starts at offset 0.
static byte *offscreen_naked;
static byte *offscreen;

// For dirty rectangles
static byte *backbuf_naked;
static byte *backbuf;

// Either `backbuf` or `offscreen`.
static byte *activebuf;

// 8x8 pixel font for debug display
static byte *pixel_font = 0;

static byte *buffer_src[kMaxVidSrc][64];

// True if offscreen completely replaced by backbuf.
// If that happens, we don't need to spot repaint or do dirty rectangles
static bool offscreen_redrawn = false;

static void init(void) {
    asm mov ax, 0x13
    asm int 0x10

    offscreen_naked = malloc(64016);
    offscreen = MK_FP(FP_SEG(offscreen_naked) + 1, 0);

    backbuf_naked = malloc(64016);
    backbuf = MK_FP(FP_SEG(backbuf_naked) + 1, 0);

    activebuf = offscreen;
}

static void shutdown(void) {
    asm mov ax, 3
    asm int 0x10

    free(offscreen_naked);
    free(backbuf_naked);
}

static void clear(void) {
    memset(activebuf, 0, 64000);
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
    offscreen_redrawn = false;
    activebuf = offscreen;
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
                    activebuf[320 * (y+row) + x + t] = color;
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

static void draw24raw(byte *src, int x, int y) {
    int max_x = (x > 296) ? (320-x) : 24;
    int max_y = (y > 176) ? (200-y) : 24;
    int seek_src = 24 - max_x;
    int seek_row = 320 - max_x;
    int start = 320 * y + x;

    // oofa

    asm push ds
    asm push es

    asm mov cx, [max_y]
    asm les di, [activebuf]
    asm lds si, [src]
    asm add di, [start]
pixrow:
    asm mov dx, [max_x]
pixcol:
    asm lodsb
    asm and al, al // 0 pixel is transparent.
    asm jnz skipload
    asm mov al, es:[di]
skipload:
    asm stosb
    asm dec dx
    asm jnz pixcol

    asm add si, [seek_src]
    asm add di, [seek_row]
    asm dec cx
    asm jnz pixrow

    asm pop es
    asm pop ds
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
    asm les di, [activebuf]
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

static void _clear24(int x, int y) {
    int start = 320 * 24 * y + 24 * x;

    asm push es
    asm xor ax, ax // Write word 0
    asm mov dx, 24 // number of rows
    asm les di, [activebuf]
    asm add di, [start]
pixrow:
    asm mov cx, 12 // use CX for `rep`. 12 words to be written.
    asm rep stosw
    asm add di, 296 // Seek to next row in buffer (320 minus 24 pixels)
    asm dec dx
    asm jnz pixrow

    asm pop es
}

static void dbg_draw_solid(byte *map) {
    int i;
    int j;
    int t;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 13; j++) {
            if (map[13 * i + j]) {
                // draw 8x8 box on top of each solid tile
                for (t = 0; t < 8; t++) {
                    memset(offscreen + (24 * 320 * i) + (320 * t) + (24 * j), 10, 8);
                }
            }
        }
    }
}

// fast = ignore transparency, good for layer 0.
static void draw_plane(int id, bool fast) {
    int i;
    int j;
    int val;
    int which;
    byte *plane = current_room[id & 3];

    // TODO: slow version
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 13; j++) {
            val = plane[13 * i + j];
            if (!val)
                continue;

            which = (val & 64) ? 1 : 0;
            draw24(which, val & 63, j, i);
        }
    }
}

static void redrawChanged(void) {
    int i;
    int layer;
    byte *tbuf;

    if (!offscreen_redrawn) {
        tbuf = activebuf;
        activebuf = backbuf;

        for (i = 0; i < tileUpdatePtr; i++) {
            int x = tileUpdateX[i];
            int y = tileUpdateY[i];
            _clear24(x, y);

            // update will go to the backbuf first,
            // then get propagated to the offscreen buf.
            // TODO: right?
            DRect_Add(x*24, y*24, x*24 + 24, y*24 + 24);

            for (layer = 0; layer < 3; layer++) {
                int val = current_room[layer][y*13 + x];
                if (val)
                    draw24(val >> 6, val & 63, x, y);
            }
        }

        activebuf = tbuf;
    }

    Screen_AckUpdates();
}

// fast = ignore transparency, good for layer 0.
static void draw_region(byte *plane, int x0, int x1, int y0, int y1, byte fast) {
    int i;
    int j;
    int val;
    int which;

    // TODO: slow version
    for (i = y0; i < y1; i++) {
        for (j = x0; j < x1; j++) {
            val = plane[13 * i + j];
            if (!val)
                continue;

            which = (val & 64) ? 1 : 0;
            draw24(which, val & 63, j, i);
        }
    }
}

static void copy_backbuf(void) {
    memcpy(offscreen, backbuf, 64000);
    offscreen_redrawn = true;
}

static void drect(void) {
    int i;
    int row;
    int len;
    rect_t *t;
    byte *src;
    byte *dst;

    if (!offscreen_redrawn) {
        for (i = 0; i < dirtyRectWritePtr; i++) {
            t = &dirtyRectangles[i];
            src = backbuf   + 320 * t->y0 + t->x0;
            dst = offscreen + 320 * t->y0 + t->x0;

            for (row = t->y0; row < t->y1; row++) {
                len = t->x1 - t->x0;
                memcpy(dst, src, len);
                src += 320;
                dst += 320;
            }
        }
    }

    dirtyRectWritePtr = 0;
}

// 1px thickness only for now.
static void strokeRect(rect_t *rect, byte color) {
    int row;
    int nrows = rect->y1 - rect->y0;
    int ncols = rect->x1 - rect->x0;
    byte *dst = activebuf + 320 * rect->y0 + rect->x0;

    // first row
    memset(dst, color, ncols);
    dst += 320;

    // left and right edge of each row.
    for (row = 1; row < (nrows-1); row++) {
        *dst = color;
        *(dst + ncols - 1) = color;
        dst += 320;
    }

    // last row
    memset(dst, color, ncols);
}

static void fillRect(rect_t *rect, byte color) {
    int row;
    int nrows = rect->y1 - rect->y0;
    int ncols = rect->x1 - rect->x0;
    byte *dst = activebuf + 320 * rect->y0 + rect->x0;

    for (row = 0; row < nrows; row++) {
        memset(dst, color, ncols);
        dst += 320;
    }
}

static void use_backbuf(bool b) {
    activebuf = b ? backbuf : offscreen;
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
    &draw24raw,
    &draw24,
    &dbg_draw_solid,
    &draw_plane,
    &redrawChanged,
    &draw_region,
    &copy_backbuf,
    &drect,
    &strokeRect,
    &fillRect,
    &use_backbuf
};
