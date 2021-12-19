#include "test/tester.h"
#include "common/types.h"
#include "map/mapfile.h"

#include <mem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int magicstr_test(void) {
    map_hdr_t hdr;
    strcpy(hdr.magicStr, " GGE file: caves");

    // Should succeed
    if (Map_VerifyMagicString(&hdr))
        return 1;

    // TODO: memset to zero struct here?

    // Greater than length of previous string. Should fail
    strcpy(hdr.magicStr, "xxxxxxxxxxxxxxxxxx");
    if (!Map_VerifyMagicString(&hdr))
        return 1;

    return 0;
}

int tilefile_test(void) {
    int result;
    map_packed_t map = {0};

    if (MapPacked_ReadFromFile(&map, "data\\elek.ggc"))
        return 1;

    // Make sure we free the data.

    result = 0;
    if (strcmp(map.hdr.tilefile[0], "ELEK1") ||
        strcmp(map.hdr.tilefile[1], "ELEK2"))
        result = 1;

    free(map.rawdata);
    return result;
}

test_t tests[] = {
    {"check magic string", &magicstr_test},
    {"read tile filenames correctly", &tilefile_test},
    {0, 0} // EOL
};

void main(void) {
    testmain(tests);
}
