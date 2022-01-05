#include "ui/msgbar.h"

#include <string.h>

#include "video/video.h"

static int counter = 0;
static int msglen = 0;

// 40 characters can fit on screen, plus one to null-terminate.
static char msgbuf[41];

// Return 1 if the counter has expired
int MsgBar_Tick(void) {
    if (counter > 0) {
        counter--;
        if (counter == 0) {
            msgbuf[0] = '\0';
            return 1;
        }
    }

    return 0;
}

void MsgBar_Set(char *msg) {
    strncpy(msgbuf, msg, 40);
    msglen = strlen(msgbuf);
    counter = 480;
}

char *MsgBar_Get(void) {
    return &msgbuf[0];
}

void MsgBar_Draw(void) {
    if (counter) {
        mydrv->type_msg(msgbuf, 8, 8, 10);
        DRect_Add(8, 8, 8 + 8 * msglen, 16);
    }
}
