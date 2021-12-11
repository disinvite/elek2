#include "v_mode_y.h"

#include <dos.h>
#include <mem.h>

#include "video.h"

#define kMaxVidSrc 8

static byte *VGA = (byte*)(0xa0000000l);

// vid_ofs points to the *inactive* video page.
static int vid_ofs = 0x4000;

// 8x8 pixel font for debug display
static byte *pixel_font = 0;

// pointer to first available block of VRAM.
// TODO: I guess this fails as you swap out banks
// because then you have fragmented memory.
static unsigned int vram_buf_ptr = 0x8000;

// video source, boolean to tell whether they are in VRAM or not
static byte  buffer_is_vram[kMaxVidSrc];
static byte *buffer_src[kMaxVidSrc][64];

static void init(void) {
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

static void shutdown(void) {
    int i;
    int j;

    asm mov ax, 3
    asm int 0x10

    // free the sprites that we gatta free
    /*
    for (i = 0; i < kMaxVidSrc; i++) {
        if (buffer_is_vram[i])
            continue;
        
        for (j = 0; j < 64; j++) {
            if (buffer_src[i][j]) {
                free(buffer_src[i][j]);
                buffer_src[i][j] = 0;
            }
        }
    }
    */
}

static void clear(void) {
    int plane;
    for (plane = 0; plane < 4; plane++) {
        // select the map mask register
        outportb(SC_INDEX, SC_MAP_MASK);

        // write 2^plane 
        outportb(SC_DATA, 1 << plane);
        
        memset(VGA + vid_ofs, 0, 16000);
    }
}

void deplane_sprites(byte **sprites, byte slot) {
    int i;
    int j;
    int plane;
    int used_sprite_idx;
    byte *spr;

    for (plane = 0; plane < 4; plane++) {
        // Pack the sprites tightly in VRAM
        used_sprite_idx = 0;

        // select the map mask register
        outportb(SC_INDEX, SC_MAP_MASK);

        // write 2^plane 
        outportb(SC_DATA, 1 << plane);

        for (i = 0; i < 64; i++) {
            spr = sprites[i];
            if (!spr)
                continue;

            for (j = 0; j < 144; j++) {
                *(VGA + vram_buf_ptr + 144 * used_sprite_idx + j) = *(spr + 4*j + plane);
            }

            used_sprite_idx++;
        }
    }

    // Now set the pointers.
    // Could do this in the first loop I guess
    used_sprite_idx = 0;
    for (i = 0; i < 64; i++) {
        if (!sprites[i])
            continue;

        buffer_src[slot][i] = (byte*)(VGA + vram_buf_ptr + 144 * used_sprite_idx);
        used_sprite_idx++;
    }

    // Seek vram pointer ahead.
    vram_buf_ptr += 144 * used_sprite_idx;
}

// set used_vram to 1 if we were able to use it
// non-plane graphics modes would set to 0 always.
static int load_sprites(byte **sprites, byte slot, byte *used_vram) {
    int i;
    int x, y, plane;
    int used_sprites;
    byte *spr;
    byte tmp[576];

    // TODO: return error if slot in use?

    /*
    // count how many sprites are in the sheet
    // so we can tell if we have the vram available.
    for (i = 0; i < 64; i++) {
        if (sprites[i])
            used_sprites++;
    }
    */
    
    /*
    // deplaned bytes. 144 bytes per sprite.
    // make sure we did not have unsigned int rollover
    if ((vram_buf_ptr + 144 * used_sprites) > 0x8000) {
        deplane_sprites(sprites, slot);
        *used_vram = 1;
    }
    */

    // just copy the pointers over. they are already allocated.
    memcpy(buffer_src[slot], sprites, 256);
    *used_vram = 0;

    // now we rotate the sprites to be column order first, by plane number
    for (i = 0; i < 64; i++) {
        spr = sprites[i];
        if (!spr)
            continue;

        memcpy(&tmp, spr, 576);

        for (plane = 0; plane < 4; plane++) {
            for (x = 0; x < 6; x++) {
                for (y = 0; y < 24; y++) {
                    *spr++ = tmp[24*y + 4*x + plane];
                }
            }
        }
    }

    buffer_is_vram[slot] = *used_vram;
    return 0;
}

static void update(void) {
    // Set CRTC start address high
    outport(0x3d4, 0xc | vid_ofs);
    vid_ofs = 0x4000 - vid_ofs;

    // wait for vblank
    while((inportb(0x3da) & 8) != 0);
    while((inportb(0x3da) & 8) == 0);
}

static void set_fontface(byte *font) {
    pixel_font = font;
}

static void type_msg(char *msg, int x, int y, byte color) {
    int i;
    int plane;
    char *tstr;
    byte *pix;
    byte *start;

    // foreach of 4 planes
    for (plane = 0; plane < 4; plane++) {
        // select the map mask register
        outportb(SC_INDEX, SC_MAP_MASK);

        // write 2^plane 
        outportb(SC_DATA, 1 << plane);
        
        start = (VGA + vid_ofs + 80 *y + x/4);
        tstr = msg;
        // foreach char in the string
        while (*tstr) {
            pix = &pixel_font[*tstr * 8];
            // foreach of 8 pixel rows
            for (i = 0; i < 8; i++) {
                if (pix[i] & (0x80 >> plane)) { *start     = color; }
                if (pix[i] & (0x08 >> plane)) { *(start+1) = color; }
                start += 80;
            }
            // rewind 8 rows, seek ahead 2 pixels
            start -= 640;
            start += 2;
            tstr++;
        }
    }
}

static void update_palette(color_t *pal) {
    int i;
    outportb(0x3c8, 0);

    for(i = 0; i < 256; i++) {
        outportb(0x3c9, pal[i].r);
        outportb(0x3c9, pal[i].g);
        outportb(0x3c9, pal[i].b);
    }
}

static void draw_vram(byte slot, byte id, int x, int y) {
    int i;
    int bx;
    byte *src;
    byte *dst;

    // deplane 24x24 coord. see comment below.
    bx = x * 6;

    src = buffer_src[slot][id];
    dst = (VGA + vid_ofs + 80 * 24 * y + bx);

    outportb(GC_INDEX, GC_MODE);
    outportb(GC_DATA, (inportb(GC_DATA) & 0xfc) | 1);

    // select the map mask register
    outportb(SC_INDEX, SC_MAP_MASK);

    // write to all 4 planes.
    outportb(SC_DATA, 15);

    for (i = 0; i < 24; i++) {
        // just do the 6 copies manually for now.
        // can't use memcpy for VRAM copy.
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        dst += 74; // 80 bytes per scanline - 6 bytes just written
    }

    outportb(GC_INDEX, GC_MODE);
    outportb(GC_DATA, inportb(GC_DATA) & 0xfc);
}

static void draw24(byte slot, byte id, int x, int y) {
    int i, j;
    int bx;
    int plane;
    byte *start;
    byte val;
    
    byte *src = buffer_src[slot][id];
    if (!src)
        return;

    if (buffer_is_vram[slot]) {
        draw_vram(slot, id, x, y);
        return;
    }

    // x is in 24x24 block coords. we need to divide by 4
    // to deplane the pixel coord.
    // this results in a multiply by 6.
    bx = x * 6;

    start = VGA + vid_ofs + 80 * 24 * y + bx;

    // AH    -- plane mask
    // DS:SI -- source sprite, ordered by column and by plane
    // ES:DI -- VGA already at correct position
    // CH -- countdown of cols to write
    // CL -- countdown of rows to write

    asm push ds
    asm push si
    asm push di

    asm les di, [start]
    asm lds si, [src]

    asm mov ah, 1
plane_loop:
    asm mov dx, 0x3c4 // SC_INDEX
    asm mov al, 2 // SC_MAP_MASK
    asm out dx, ax

    asm mov ch, 6
col_loop:

        asm mov cl, 24
    row_loop:
            asm lodsb
            asm or al, al
            asm jz skip_write
            asm stosb
            asm dec di
        skip_write:
            asm add di, 80
            asm dec cl
            asm jnz row_loop

        asm sub di, 1919 // (80*24 - 1)

        asm dec ch
        asm jnz col_loop

    asm sub di, 6

    asm shl ah, 1
    asm and ah, 15
    asm jnz plane_loop

    asm pop di
    asm pop si
    asm pop ds
}

static void dbg_draw_solid(byte *map) {
    
}

static void draw_plane(byte *plane, byte fast) {
    
}

video_drv_t mode_y_drv = {
    &init,
    &shutdown,
    &clear,
    &update,
    &load_sprites,
    &set_fontface,
    &type_msg,
    &update_palette,
    &draw24,
    &dbg_draw_solid,
    &draw_plane
};

