#include "sprite.h"

#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

// big block of memory for each sprite sheet
static byte *rawpixel[kMaxSpriteSlots];
static byte *rawstatus[kMaxSpriteSlots];

// pointers to each individual sprite.
static byte *loaded_sprites[kMaxSpriteSlots][64];
byte *Sprite_Status[kMaxSpriteSlots][64];

byte **getSpriteSlot(int slot) {
    return loaded_sprites[slot];
}

// `id` is the sprite number from 0-63
// `pos` is the actual number of sprites read
// the benefit is so that we malloc the entire memory chunk at once
// and don't waste any by holding space for non-existent sprites.
void unpack_sprite(int slot, int id, int pos, gge_sprite_t *spr) {
    int i;
    byte ph;
    byte pl;
    byte *buf = rawpixel[slot] + 576 * pos;
    loaded_sprites[slot][id] = buf;

    for (i = 0; i < 288; i++) {
        pl = spr->VGA_color[spr->packed[i] & 15];
        ph = spr->VGA_color[(spr->packed[i] >> 4) & 15];
        buf[2 * i] = ph;
        buf[2 * i + 1] = pl;
    }
}

void free_sprites(void) {
    int slot;
    int i;
    for (slot = 0; slot < kMaxSpriteSlots; slot++) {
        if (rawpixel[slot]) {
            free(rawpixel[slot]);
            rawpixel[slot] = 0;
        }

        if (rawstatus[slot]) {
            free(rawstatus[slot]);
            rawstatus[slot] = 0;
        }

        for (i = 0; i < 64; i++) {
            loaded_sprites[slot][i] = 0;
            Sprite_Status[slot][i] = 0;
        }
    }
}

void readGGS(char *filename, int slot) {
    byte used;
    int future_size;
    int i;
    int n_sprites;
    long size;
    gge_sprite_t spr;
    byte statusTable[512];
    byte sizeTable[256];

    FILE *f = fopen(filename, "rb");

    // Seek to end to calculate total size.
    fseek(f, 0, 2);
    size = ftell(f);

    // Skip header
    fseek(f, 96, 0);
    fread(&statusTable, 64, 8, f);
    fread(&sizeTable,   64, 4, f);
    
    // Skip future section
    fread(&future_size, sizeof(future_size), 1, f);
    fseek(f, future_size, 1); // SEEK_SET

    n_sprites = (size - ftell(f)) / sizeof(spr);
    rawpixel[slot] = malloc(n_sprites * 576);
    rawstatus[slot] = malloc(n_sprites * 8);

    n_sprites = 0;
    for(i = 0; i < 64; i++) {
        fread(&used, 1, 1, f);
        if (used == 255)
            continue;

        fread(&spr, sizeof(spr), 1, f);
        unpack_sprite(slot, i, n_sprites, &spr);
        Sprite_Status[slot][i] = rawstatus[slot] + 8 * n_sprites;
        memcpy(Sprite_Status[slot][i], &statusTable[8 * i], 8);

        n_sprites++;
    }

    fclose(f);
}

