#include "common/types.h"
#include "video/video.h"

#include <stdio.h>

video_drv_t *mydrv = 0;

rect_t dirtyRectangles[kMaxDirtyRect];
int dirtyRectWritePtr = 0;

static byte dbg_font[128][8];

void V_LoadFont(char *filename) {
    FILE *f = fopen(filename, "rb");
    
    if (!f)
        return;

    if (fread(&dbg_font, 8, 128, f) != 128) {
        fclose(f);
        return;
    }
    fclose(f);

    mydrv->set_fontface(&dbg_font);
}

void V_LoadPal(void) {
    color_t pal[256];
    FILE *f = fopen("data/em.pal", "rb");
    fread(&pal, 3, 256, f);
    fclose(f);

    mydrv->update_palette(pal);
}

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

    // clamp to screen
    if (x0 < 0)   x0 = 0;
    if (x1 > 320) x1 = 320;
    if (y0 < 0)   y0 = 0;
    if (y1 > 200) y1 = 200;
    
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
