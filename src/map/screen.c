#include "common/types.h"
#include "map/screen.h"

#include <mem.h>

static byte layer_temp[kRoomSize];

// [r*13 + c for c in range(13) for r in range(8)]
static byte kLutHV[kRoomSize] = {
    0,  13, 26, 39, 52, 65, 78, 91,
    1,  14, 27, 40, 53, 66, 79, 92,
    2,  15, 28, 41, 54, 67, 80, 93,
    3,  16, 29, 42, 55, 68, 81, 94,
    4,  17, 30, 43, 56, 69, 82, 95,
    5,  18, 31, 44, 57, 70, 83, 96,
    6,  19, 32, 45, 58, 71, 84, 97,
    7,  20, 33, 46, 59, 72, 85, 98,
    8,  21, 34, 47, 60, 73, 86, 99,
    9,  22, 35, 48, 61, 74, 87, 100,
    10, 23, 36, 49, 62, 75, 88, 101,
    11, 24, 37, 50, 63, 76, 89, 102,
    12, 25, 38, 51, 64, 77, 90, 103
};

// [r*8 + c for c in range(8) for r in range(13)]  
static byte kLutVH[kRoomSize] = {
    0,  8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96,
    1,  9, 17, 25, 33, 41, 49, 57, 65, 73, 81, 89, 97,
    2, 10, 18, 26, 34, 42, 50, 58, 66, 74, 82, 90, 98,
    3, 11, 19, 27, 35, 43, 51, 59, 67, 75, 83, 91, 99,
    4, 12, 20, 28, 36, 44, 52, 60, 68, 76, 84, 92, 100,
    5, 13, 21, 29, 37, 45, 53, 61, 69, 77, 85, 93, 101,
    6, 14, 22, 30, 38, 46, 54, 62, 70, 78, 86, 94, 102,
    7, 15, 23, 31, 39, 47, 55, 63, 71, 79, 87, 95, 103
};

// Room data for all users of the API
byte current_room[4][kRoomSize];

// Index of which tiles which have been updated since the last acknowledgement
// Intended for surgical screen buffer updates, but I guess it might have
// some other use.
// If you exceed some threshhold, is it cheaper to redraw the entire screen?
// TODO: static?
int tileUpdateX[kMaxTileUpdate] = {0};
int tileUpdateY[kMaxTileUpdate] = {0};
int tileUpdatePtr = 0;

// Mark grid tiles that should be redrawn from scratch
// prior to screen update.
// Return error if we have hit the max for updates this frame
int Screen_TileChanged(int x, int y) {
    if (tileUpdatePtr == kMaxTileUpdate)
        return 1;

    tileUpdateX[tileUpdatePtr] = x;
    tileUpdateY[tileUpdatePtr] = y;
    tileUpdatePtr++;

    return 0;
}

void Screen_AckUpdates(void) {
    tileUpdatePtr = 0;
}

void Layer_htov(byte *src) {
    int i;
    
    for (i = 0; i < kRoomSize; i++)
        layer_temp[i] = src[kLutHV[i]];

    memcpy(src, layer_temp, kRoomSize);
}

void Layer_vtoh(byte *src) {
    int i;
    
    for (i = 0; i < kRoomSize; i++)
        layer_temp[i] = src[kLutVH[i]];

    memcpy(src, layer_temp, kRoomSize);
}

void Layer_Unpack(byte *src, byte *dst) {
    int i;
    byte b;
    byte rpt;
    byte vertical;

    // Unpack horizontal.
    // Check whether we need to rotate when we're done.
    vertical = (*src++) & 128;
    i = 0;

    while (i < kRoomSize) {
        b = *src++;
        if (b & 128) {
            // plus one, very important.
            rpt = *src++ + 1;

            // don't write too much
            if (rpt + i > kRoomSize)
                rpt = kRoomSize - i;

            memset(&dst[i], b & 127, rpt);
            i += rpt;
        } else {
            dst[i] = b & 127;
            i++;
        }
    }

    if (vertical)
        Layer_vtoh(dst);
}

int Layer_Pack(byte *src, byte *dst) {
}

int Layer_PackVert(byte *src, byte *dst) {
}

int Layer_PackHoriz(byte *src, byte *dst) {
}
