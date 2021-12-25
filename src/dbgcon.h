#ifndef FONT8_H
#define FONT8_H

#include "common/types.h"
#include "video/video.h"

void DbgCon_Init(void);
void DbgCon_Close(void);
void DbgCon_Insert(char *msg);
void DbgCon_Draw(void);
int DbgCon_Tick(int secs);

#endif
