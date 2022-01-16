#ifndef TYPES_H
#define TYPES_H

typedef unsigned char byte;
typedef enum {false, true} bool;

typedef struct rect_s {
    int x0, y0, x1, y1;
} rect_t;

#endif
