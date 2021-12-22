#include "map/mapfile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Map_VerifyMagicString(map_hdr_t *hdr) {
    return strncmp(hdr->magicStr, kGGECavesString, 16) ? 1 : 0;
}

int MapPacked_ReadFromFile(map_packed_t *mapfile, char *filename) {
    FILE *f;
    int i;
    int future_len;
    int temp_offset[8];
    long mapFileLength;
    long mapDataLength;
    byte *rawdata;

    // Don't overwrite already allocated buf
    if (mapfile->rawdata)
        return -1;

    f = fopen(filename, "rb");

    // If we didn't get a file
    if (!f)
        return -2;

    // Skip to end, get total length, then rewind.
    // We need this to know how much screen data to expect.
    fseek(f, 0, SEEK_END);
    mapFileLength = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Read entire header all at once.
    fread(&mapfile->hdr, sizeof(map_hdr_t), 1, f);

    // Reserved for future use. Read the length and just skip this section.
    fread(&future_len, 2, 1, f);
    fseek(f, ftell(f) + future_len, SEEK_SET);

    // Read the layer offsets for each room. Skip any rooms that don't exist.
    for (i = 0; i < kMaxRooms; i++) {
        if (!mapfile->hdr.roomExists[i])
            continue;

        // Space for 8 offsets (16 bytes), but only half are used.
        fread(temp_offset, 2, 8, f);
        mapfile->offsets[i][0] = temp_offset[0];
        mapfile->offsets[i][1] = temp_offset[1];
        mapfile->offsets[i][2] = temp_offset[2];
        mapfile->offsets[i][3] = temp_offset[3];
    }

    // Prepare to read the remainder of the file.
    mapDataLength = mapFileLength - ftell(f);
    rawdata = malloc(mapDataLength);
    
    // Fail if alloc failed
    if (!rawdata) {
        fclose(f);
        return -3;
    }

    if (fread(rawdata, mapDataLength, 1, f) != 1) {
        free(rawdata);
        fclose(f);
        return -4;
    }

    mapfile->rawdata = rawdata;
    fclose(f);
    return 0;
}

void MapPacked_FreeData(map_packed_t *mapfile) {
    if (mapfile->rawdata)
        free(mapfile->rawdata);

    mapfile->rawdata = 0;
    // TODO: could memset some things here I guess
}

void MapPacked_LoadRoom(map_packed_t *mapfile, int id, layer_ptr_t *layers) {
    int i;

    if (!mapfile->hdr.roomExists[id]) {
        // No room here. All pointers to zero and we're done
        memset(layers, sizeof(*layers), 0);
        return;
    }

    // TODO: 2048 base offset assumes there's nothing in the future section

    for (i = 0; i < 4; i++) {
        if (mapfile->offsets[id][i])
            layers->layer[i] = mapfile->rawdata + mapfile->offsets[id][i] - 2048;
        else
            layers->layer[i] = 0;
    }
}

