#ifndef VIDEO_H
#define VIDEO_H

typedef unsigned char byte;

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
    void (*draw24)(byte slot, byte id, int x, int y);
} video_drv_t;

// int load_sprites(byte **sprites, byte slot, byte *used_vram)
// void (*draw24)(byte slot, byte id, int x, int y)

#endif
