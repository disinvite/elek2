#ifndef TEST_CASE_SCR_H
#define TEST_CASE_SCR_H

// Test cases for t_scr.c

#include "common/types.h"

// Flags for packing orientation. Should be first byte of packed data.
#define packedH 0
#define packedV 128

// Helper for packed run-length byte `val` for `count` times.
#define Repeat(val, count) 128 | val, (count - 1)

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

byte kPackTestEasy[104] = {
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};

byte kUnpackTestEasyH[] = { packedH, Repeat(5, 104) };
byte kUnpackTestEasyV[] = { packedV, Repeat(5, 104) };

byte kPackTestPreferHoriz[] = {
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

byte kUnpackPreferHoriz[] = { packedH,
    Repeat(5,13), Repeat(1,13),
    Repeat(5,13), Repeat(1,13),
    Repeat(5,13), Repeat(1,13),
    Repeat(5,13), Repeat(1,13)
};

byte kPackTestPreferVert[] = {
    5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5,
    5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5,
    5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5,
    5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5,
    5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5,
    5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5,
    5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5,
    5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5
};

byte kUnpackPreferVert[] = { packedV,
    Repeat(5,8), Repeat(1,8),
    Repeat(5,8), Repeat(1,8),
    Repeat(5,8), Repeat(1,8),
    Repeat(5,8), Repeat(1,8),
    Repeat(5,8), Repeat(1,8),
    Repeat(5,8), Repeat(1,8),
    Repeat(5,8)
};

#undef packedH
#undef packedV
#undef Repeat

#endif
