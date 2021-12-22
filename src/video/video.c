#include "common/types.h"
#include "video/video.h"

rect_t dirtyRectangles[kMaxDirtyRect];
int dirtyRectWritePtr = 0;

void DRect_Reset(void) {
    dirtyRectWritePtr = 0;    
}

int DRect_Any(void) {
    return dirtyRectWritePtr > 0;
}

int DRect_Add(int x0, int y0, int x1, int y1) {
    rect_t *t = &dirtyRectangles[dirtyRectWritePtr];

    // fail on invalid rectangle
    if (x1 < x0 || y1 < y0)
        return -1;
    
    // fail if we've got too many rectangles
    if (dirtyRectWritePtr == kMaxDirtyRect)
        return -1;

    t->x0 = x0;
    t->y0 = y0;
    t->x1 = x1;
    t->y1 = y1;
    dirtyRectWritePtr++;

    return 0;
}
