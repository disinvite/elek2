#ifndef MAP_SCREEN_H
#define MAP_SCREEN_H

void Layer_htov(byte *src);
void Layer_vtoh(byte *src);
void Layer_Unpack(byte *src, byte *dst);
int Layer_Pack(byte *src, byte *dst);
int Layer_PackVert(byte *src, byte *dst);
int Layer_PackHoriz(byte *src, byte *dst);

#endif