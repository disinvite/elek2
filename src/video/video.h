#ifndef VIDEO_VIDEO_H
#define VIDEO_VIDEO_H

#include "common/types.h"

#define kMaxDirtyRect 100

typedef struct color_s {
    byte r,g,b;
} color_t;

typedef struct video_drv_s {
    void (*init)(void);
    void (*shutdown)(void);
    void (*clear)(void);
    void (*update)(void);
    int  (*load_sprites)(byte **sprites, byte slot, byte *used_vram);
    void (*set_fontface)(byte *font);
    void (*type_msg)(char *msg, int x, int y, byte color);
    void (*update_palette)(color_t *pal);
    void (*draw24raw)(byte *src, int x, int y);
    void (*draw24)(byte slot, byte id, int x, int y);
    void (*dbg_draw_solid)(byte *map);
    void (*draw_plane)(int id, bool fast);
    void (*redrawChanged)(void);
    void (*draw_region)(byte *plane, int x0, int x1, int y0, int y1, byte fast);
    void (*copy_backbuf)(void);
    void (*drect)(void);
    void (*strokeRect)(rect_t *rect, byte color);
    void (*fillRect)(rect_t *rect, byte color);
    void (*use_backbuf)(bool b);
    void (*darken)(bool darker);
    void (*outline_tile)(int x, int y, int option);
} video_drv_t;

extern video_drv_t *mydrv;

extern rect_t dirtyRectangles[kMaxDirtyRect];
extern int dirtyRectWritePtr;

void V_LoadFont(char *filename);
void V_LoadPal(void);
void DRect_Reset(void);
int DRect_Any(void);
int DRect_Add(int x0, int y0, int x1, int y1);

#endif
