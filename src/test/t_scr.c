#include "test/tester.h"
#include "common/types.h"
#include "map/screen.h"

#include <mem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

byte kRandomOrder[104] = {
    46, 55, 23, 87, 81, 59, 11,  47,  51,   7, 42, 86, 43,
    94, 9,  32, 18,  4, 76, 30,  44,  27, 102, 57, 58, 82,
    68, 65, 28, 16, 95, 25, 31, 103, 101,  35, 49, 33, 63,
    80, 21, 75, 14,  0, 20, 91,  62,  84,  12, 70, 67, 41,
    13, 83, 74, 8,   1, 98, 88,  79,  92,  45, 97,  5, 37,
    29, 40, 26, 89,  2, 99, 60,  50, 100,  56, 90, 96, 48,
    24, 19, 17, 22,  3, 52, 64,  66,  38,  73, 93, 85, 36,
    78, 34, 71, 15, 54, 10, 69,   6,  77,  39, 53, 72, 61
};

byte kRandomOrderRot[104] = {
    46,  94,  68, 80, 13,  29, 24, 78,
    55,   9,  65, 21, 83,  40, 19, 34,
    23,  32,  28, 75, 74,  26, 17, 71,
    87,  18,  16, 14,  8,  89, 22, 15,
    81,   4,  95,  0,  1,   2,  3, 54,
    59,  76,  25, 20, 98,  99, 52, 10,
    11,  30,  31, 91, 88,  60, 64, 69,
    47,  44, 103, 62, 79,  50, 66,  6,
    51,  27, 101, 84, 92, 100, 38, 77,
     7, 102,  35, 12, 45,  56, 73, 39,
    42,  57,  49, 70, 97,  90, 93, 53,
    86,  58,  33, 67,  5,  96, 85, 72,
    43,  82,  63, 41, 37,  48, 36, 61
};

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

test_t tests[] = {
    {"rotate H to V", &rotateHV_test},
    {"rotate V to H", &rotateVH_test},
    {0, 0} // EOL
};

void main(void) {
    testmain(tests);
}
