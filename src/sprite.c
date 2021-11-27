#include "sprite.h"

#include <stdio.h>
#include <stdlib.h>

static byte *loaded_sprites[4][64];

byte **getSpriteSlot(int slot) {
    return loaded_sprites[slot];
}

void unpack_sprite(int slot, int id, gge_sprite_t *spr) {
    int i;
    byte ph;
    byte pl;
    byte *buf = malloc(576);
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
    for (slot = 0; slot < 4; slot++) {
        for (i = 0; i < 64; i++) {
            if (loaded_sprites[slot][i]) {
                free(loaded_sprites[slot][i]);
                loaded_sprites[slot][i] = 0;
            }
        }
    }
}

void readGGC(char *filename, int slot) {
    byte _nul;
    byte used;
    int future_size;
    int i;
    gge_sprite_t spr;

    FILE *f = fopen(filename, "rb");

    // Skip header, status table, size table
    fseek(f, 96 + 512 + 256, 0);
    
    // Skip future section
    fread(&future_size, sizeof(future_size), 1, f);
    for (i = 0; i < future_size; i++) {
        fread(&_nul, 1, 1, f);
    }

    for(i = 0; i < 64; i++) {
        fread(&used, 1, 1, f);
        if (used == 255)
            continue;

        fread(&spr, sizeof(spr), 1, f);
        unpack_sprite(slot, i, &spr);
    }

    fclose(f);
}

