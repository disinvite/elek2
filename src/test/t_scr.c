#include "test/tester.h"
#include "common/types.h"
#include "map/screen.h"
#include "test/case/tc_scr.h"

#include <mem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int memncmp(byte *buf0, byte *buf1, int n) {
    while (n--) {
        if (*buf0++ != *buf1++)
            return 1;
    }

    return 0;
}

// simple test of helper method
int memncmp_test(void) {
    byte scene[104];
    memcpy(scene, kPackTestEasy, 104);

    if (memncmp(scene, kPackTestEasy, 104))
        return 1;

    scene[103] = 0;

    if (!memncmp(scene, kPackTestEasy, 104))
        return 1;

    return 0;
}

int rotateHV_test(void) {
    int i;
    byte test[104];

    memcpy(test, kRandomOrder, 104);
    Layer_htov(test);

    for (i = 0; i < 104; i++) {
        if (test[i] != kRandomOrderRot[i])
            return 1;
    }

    return 0;
}

int rotateVH_test(void) {
    int i;
    byte test[104];

    memcpy(test, kRandomOrderRot, 104);
    Layer_vtoh(test);

    for (i = 0; i < 104; i++) {
        if (test[i] != kRandomOrder[i])
            return 1;
    }

    return 0;
}

int unpackEasy_test(void) {
    byte scene[104];

    // Vertical
    memset(scene, 0, 104);
    Layer_Unpack(kUnpackTestEasyV, &scene);
    if (memncmp(scene, kPackTestEasy, 104))
        return 1;

    // Horizontal
    memset(scene, 0, 104);
    Layer_Unpack(kUnpackTestEasyH, &scene);
    if (memncmp(scene, kPackTestEasy, 104))
        return 1;

    return 0;
}

int unpackHoriz_test(void) {
    byte scene[104];

    memset(scene, 0, 104);
    Layer_Unpack(kUnpackPreferHoriz, &scene);
    
    if (memncmp(kPackTestPreferHoriz, scene, 104))
        return 1;

    return 0;
}

int unpackVert_test(void) {
    byte scene[104];

    memset(scene, 0, 104);
    Layer_Unpack(kUnpackPreferVert, &scene);

    if (memncmp(kPackTestPreferVert, scene, 104))
        return 1;

    return 0;
}

test_t tests[] = {
    {"~memncmp helper function", &memncmp_test},

    {"rotate H to V", &rotateHV_test},
    {"rotate V to H", &rotateVH_test},

    {"simple unpack", &unpackEasy_test},
    {"horiz unpack test", &unpackHoriz_test},
    {"vert unpack test", &unpackVert_test},
    {0, 0} // EOL
};

void main(void) {
    testmain(tests);
}
