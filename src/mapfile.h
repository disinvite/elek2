#ifndef MAPFILE_H
#define MAPFILE_H

typedef unsigned char byte;

extern byte current_room[4][8][13];
extern byte collision[8][13];

void map_load(char *filename);
void map_decode(int room);
void map_free(void);

#endif
