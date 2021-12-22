#include "common/types.h"
#include "dbgcon.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "video/video.h"

typedef struct dbg_entry_s {
    char *msg;
    int tics;
    struct dbg_entry_s *next;
} dbg_entry_t;

// 128 chars, 8 bytes per character.
#define kDbgMaxSize 6
static byte dbg_font[128][8];

// function pointers into the video driver
static video_drv_t *mydrv;

static dbg_entry_t dbg_stack[kDbgMaxSize];
static dbg_entry_t *dStart;
static dbg_entry_t *dEnd;
static byte dbg_setup = 0;

static int n_called = 0;
static int stored_secs;
static char fpsbuf[4];

static bool dbg_redraw = false;

void DbgCon_Init(char *filename, video_drv_t *_drv) {
    int i;

    FILE *f = fopen(filename, "rb");
    if (!f)
        return;
    if (fread(&dbg_font, 8, 128, f) != 128) {
        fclose(f);
        return;
    }
    fclose(f);

    mydrv = _drv;
    mydrv->set_fontface(&dbg_font);

    dStart = &dbg_stack[0];
    dEnd = dStart;
    for (i = 0; i < kDbgMaxSize - 1; i++)
        dbg_stack[i].next = &dbg_stack[i+1];
    dbg_stack[kDbgMaxSize - 1].next = &dbg_stack[0];

    dbg_setup = 1;
}

void DbgCon_Close(void) {
    int i;
    for (i = 0; i < kDbgMaxSize; i++) {
        if (dbg_stack[i].msg)
            free(dbg_stack[i].msg);
    }

    mydrv->set_fontface(0);
}

void DbgCon_Insert(char *msg) {
    if (dEnd->msg)
        free(dEnd->msg);

    dEnd->msg = strdup(msg);
    dEnd->tics = 100;
    dEnd = dEnd->next;

    // if overwrite
    if (dStart == dEnd && dStart->tics > 0) {
        free(dStart->msg);
        dStart->msg = 0;
        dStart->tics = 0;
        dStart = dStart->next;
    }

    dbg_redraw = true;
}

void DbgCon_Draw(void) {
    int i;

    dbg_entry_t *t = dStart;
    for (i = 0; i < kDbgMaxSize; i++) {
        if (t == dEnd)
            break;
        mydrv->type_msg(t->msg, 11, 11 + 10*i, 0);
        mydrv->type_msg(t->msg, 10, 10 + 10*i, 2);
        t = t->next;
    }

    mydrv->type_msg(fpsbuf, 281, 11, 0);
    mydrv->type_msg(fpsbuf, 280, 10, 4);

    dbg_redraw = false;
}

int DbgCon_Tick(int secs) {
    // fps counter math
    n_called++;
    if (stored_secs != secs) {
        // TODO: eliminate sprintf.
        sprintf(fpsbuf, "%02d", n_called);
        stored_secs = secs;
        n_called = 0;
        dbg_redraw = true;
    }

    // flip flop, should draw every frame.
    mydrv->type_msg(".", 312, 10, (n_called & 31) ? 4 : 0);

    dStart->tics--;
    if (dStart->tics == 0) {
        free(dStart->msg);
        dStart->msg = 0;
        dStart = dStart->next;
        dbg_redraw = true;
    }

    return dbg_redraw;
}
