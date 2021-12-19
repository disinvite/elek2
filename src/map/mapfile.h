#ifndef MAP_MAPFILE_H
#define MAP_MAPFILE_H

#include "common/types.h"

#define kGGECavesString " GGE file: caves"
#define kMaxRooms 256

typedef struct {
    char magicStr[32];
    char tilefile[2][9];
    byte params[16];
    byte roomExists[kMaxRooms];
} map_hdr_t;

typedef struct {
    map_hdr_t hdr;
    int offsets[kMaxRooms][4];
    byte *rawdata;
} map_packed_t;

typedef struct {
    byte *layer[4];
} layer_ptr_t;

int Map_VerifyMagicString(map_hdr_t *hdr);

int MapPacked_ReadFromFile(map_packed_t *mapfile, char *filename);
void MapPacked_FreeData(map_packed_t *mapfile);
void MapPacked_LoadRoom(map_packed_t *mapfile, int id, layer_ptr_t *layers);

#endif
