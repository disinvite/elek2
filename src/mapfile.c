#include "mapfile.h"

#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

static byte room_exists[256];
static int plane0_ofs[256];

byte current_room[8][13];

static byte *rawmap;

void map_load(char *filename) {
    byte _nul[8];
    int future_size;
    int i;
    int _ofs[8]; // array of 8 but we only use 4.
    long filelength;
    long remainder;

    FILE *f = fopen(filename, "rb");
    fseek(f, 0, 2);

    filelength = ftell(f);
    fseek(f, 0, 0);

    // Skip header, sprite names, params
    fseek(f, 32 + 18 + 16, 0);
    
    fread(&room_exists, 256, 1, f);

    // Skip future section
    fread(&future_size, sizeof(future_size), 1, f);
    for (i = 0; i < future_size; i++) {
        fread(&_nul, 1, 1, f);
    }

    for(i = 0; i < 256; i++) {
        if (room_exists[i] == 0)
            continue;

        fread(&_ofs, 2, 8, f);
        plane0_ofs[i] = _ofs[0]; // TODO
    }

    remainder = filelength - ftell(f);
    rawmap = malloc(remainder);
    fread(rawmap, remainder, 1, f);

    fclose(f);
}

void map_decode(int room) {
    int i, j;
    byte *ptr;
    byte vertical;
    byte buf[13 * 8];
    byte rpt;
    byte b;

    if (!room_exists[room]) {
        memset(current_room, 0, 13*8);
        return;
    }

    ptr = &rawmap[plane0_ofs[room] - 2048];
    vertical = (*ptr) & 128;
    ptr++;
    i = 0;

    while (i < 104) {
        b = *ptr++;
        if (b & 128) {
            // plus one, very important.
            rpt = *ptr++ + 1;

            // don't write too much
            if (rpt + i > 104)
                rpt = 104 - i;

            memset(&buf[i], b & 127, rpt);
            i += rpt;
        } else {
            buf[i] = b & 127;
            i++;
        }
    }

    if (vertical) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 13; j++) {
                current_room[i][j] = buf[8*j + i];
            }
        }
    } else {
        memcpy(current_room, buf, 104);
    }
}

void map_free(void) {
    free(rawmap);
}
