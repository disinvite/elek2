#include "mapfile.h"

#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#include "sprite.h"

static byte room_exists[256];
static int plane_ofs[256][4];

byte current_room[4][8][13];
byte collision[8][13];

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
        plane_ofs[i][0] = _ofs[0];
        plane_ofs[i][1] = _ofs[1];
        plane_ofs[i][2] = _ofs[2];
        plane_ofs[i][3] = _ofs[3];
    }

    remainder = filelength - ftell(f);
    rawmap = malloc(remainder);
    fread(rawmap, remainder, 1, f);

    fclose(f);
}

static void calc_solid(void) {
    int i;
    int j;
    int plane;
    byte val;
    byte status;
    //memset(collision, 0, sizeof(collision));

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 13; j++) {
            for (plane = 3; plane >= 0; plane--) {
                val = current_room[plane][i][j];
                status = Sprite_Status[(val >> 6) & 1][val & 63][0];
                collision[i][j] = status & 0x80;
                
                if (status & 0x80)
                    break;
            }
        }
    }

}

void map_decode(int room) {
    int i, j;
    int plane;
    byte *ptr;
    byte vertical;
    byte buf[13 * 8];
    byte rpt;
    byte b;

    memset(current_room, 0, 4*13*8);
    
    if (!room_exists[room])
        return;

    for (plane = 0; plane < 4; plane++) {
        if (!plane_ofs[room][plane])
            continue;

        memset(buf, 0, 104);

        ptr = &rawmap[plane_ofs[room][plane] - 2048];
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
                    current_room[plane][i][j] = buf[8*j + i];
                }
            }
        } else {
            memcpy(current_room[plane], buf, 104);
        }
    }

    calc_solid();
}

void map_free(void) {
    free(rawmap);
}
