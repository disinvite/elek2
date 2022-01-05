#ifndef UI_MSGBAR_H
#define UI_MSGBAR_H

int MsgBar_Tick(void);
void MsgBar_Set(char *msg);
char *MsgBar_Get(void);
void MsgBar_Draw(void);

#endif
