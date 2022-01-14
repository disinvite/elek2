#include "common/string.h"

#include "common/types.h"

#include <string.h>

void string_getdim(char *str, int *x, int *y) {
    char *tstart;
    char *tend;
    bool lastline;
    int tlen;
    char buf[150];

    // to protect against an over-long string
    strncpy(buf, str, 149);
    buf[149] = '\0';

    *x = 1;
    *y = 1;

    tstart = buf;
    lastline = false;
    while (tstart) {
        // Find the next newline
        tend = strchr(tstart, '\n');
        
        // If we didn't find it, this is the last line
        lastline = !tend;

        // Get strlen up to this newline character.
        *tend = '\0';
        tlen = strlen(tstart);

        // Bump up the max
        if (tlen > *x)
            *x = tlen;

        if (lastline)
            break;

        // Seek ahead
        (*y)++;
        tstart = tend + 1;
    }
}

