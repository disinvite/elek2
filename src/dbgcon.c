#include "dbgcon.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dbg_entry_s {
    char *msg;
    int tics;
    struct dbg_entry_s *next;
} dbg_entry_t;

// 128 chars, 8 bytes per character.
#define kDbgMaxSize 6
static byte dbg_font[128][8];
static dbg_entry_t dbg_stack[kDbgMaxSize];
static dbg_entry_t *dStart;
static dbg_entry_t *dEnd;
static byte dbg_setup = 0;

static int n_called = 0;
static int stored_secs;
static char fpsbuf[4];

void DbgCon_Init(char *filename) {
    int i;

    FILE *f = fopen(filename, "rb");
    if (!f)
        return;
    if (fread(&dbg_font, 8, 128, f) != 128) {
        fclose(f);
        return;
    }
    fclose(f);

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
}

static void DbgCon_Type(byte *buf, char *msg, int x, int y, byte color) {
    int i;
    int row;
    byte b;
    int ofs;
    int t;
    int len;

    if (!dbg_setup)
        return;

    //byte *start = &buf[320 * y];
    len = strlen(msg);

    for (i = 0; i < len; i++) {
        ofs = msg[i] & 127;
        for (row = 0; row < 8; row++) {
            b = dbg_font[ofs][row];
            for (t = 0; t < 8; t++) {
                if (b&128)
                    buf[320 * (y+row) + x + t] = color;
                    //*(start + 320*row + x + t) = color;
                b <<= 1;
            }
        }

        x += 8;
        // TODO: wrapping or whatever?
        if (x > 312)
            break;
    }
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
}

void DbgCon_Draw(byte *buf, int secs) {
    int i;
    char flipflop[2];

    dbg_entry_t *t = dStart;
    for (i = 0; i < kDbgMaxSize; i++) {
        if (t == dEnd)
            break;
        DbgCon_Type(buf, t->msg, 11, 11 + 10*i, 0);
        DbgCon_Type(buf, t->msg, 10, 10 + 10*i, 2);
        t = t->next;
    }

    n_called++;
    if (stored_secs != secs) {
        sprintf(fpsbuf, "%02d", n_called);
        stored_secs = secs;
        n_called = 0;
    }

    flipflop[0] = (n_called & 31) ? '.' : ' ';

    DbgCon_Type(buf, flipflop, 313, 11, 0);
    DbgCon_Type(buf, flipflop, 312, 10, 4);

    DbgCon_Type(buf, fpsbuf, 281, 11, 0);
    DbgCon_Type(buf, fpsbuf, 280, 10, 4);
}

void DbgCon_Tick(void) {
    dStart->tics--;
    if (dStart->tics == 0) {
        free(dStart->msg);
        dStart->msg = 0;
        dStart = dStart->next;
    }
}
