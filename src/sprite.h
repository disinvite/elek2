#ifndef SPRITE_H
#define SPRITE_H

typedef unsigned char byte;

typedef struct gge_sprite_s {
    byte future[8];
    byte packed[288];
    byte VGA_color[48];
    byte EGA_color[32];
    byte CGA_color[32];
    byte v16_color[32];
    byte v32_color[48];
} gge_sprite_t;

byte **getSpriteSlot(int slot);
void free_sprites(void);
void readGGC(char *filename, int slot);

#endif
