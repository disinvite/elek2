#ifndef FONT8_H
#define FONT8_H

#include "video/video.h"

typedef unsigned char byte;

void DbgCon_Init(char *filename, video_drv_t *drv);
void DbgCon_Close(void);
void DbgCon_Insert(char *msg);
void DbgCon_Draw(int secs);
void DbgCon_Tick(void);

#endif
