#ifndef FONT8_H
#define FONT8_H

typedef unsigned char byte;

void DbgCon_Init(char *filename);
void DbgCon_Close(void);
void DbgCon_Insert(char *msg);
void DbgCon_Draw(byte *buf, int secs);
void DbgCon_Tick(void);

#endif
