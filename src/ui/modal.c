#include "ui/modal.h"

#include "common/string.h"
#include <string.h>

#include "video/video.h"

// padding around the entire window
#define kModalPadding 20

// padding inside/between each button
#define kButtonPadding 8


void Modal_Draw(modal_t *modal) {
    int i;
    int msgx, msgy;
    int pixx, pixy;
    char buf[150];
    char *tstart;
    char *tend;

    rect_t rect = {0, 30, 0, 0};
    rect_t ok_rect = {0, 0, 0, 0};

    strncpy(buf, modal->text, 149);
    buf[149] = '\0';

    // figure out how big the box should be
    string_getdim(modal->text, &msgx, &msgy);

    // convert char dims to pixel unit.
    pixx = msgx * 8;
    pixy = msgy * 8;

    rect.x0 = 160 - (pixx/2 + kModalPadding);
    rect.x1 = rect.x0 + (2*kModalPadding) + pixx;
    rect.y1 = rect.y0 + (3*kModalPadding) + 8 + (2*kButtonPadding) + pixy;

    ok_rect.x0 = rect.x0 + kModalPadding;
    ok_rect.x1 = ok_rect.x0 + (2*kButtonPadding) + 16;
    ok_rect.y0 = rect.y1 - (kModalPadding + 2*kButtonPadding + 8);
    ok_rect.y1 = rect.y1 - kModalPadding;

    // prepare the stage for drawing the modal
    mydrv->use_backbuf(true);
    mydrv->darken(true);

    // draw the box for the window
    mydrv->fillRect(&rect, 3);
    mydrv->strokeRect(&rect, 5);

    // type main modal text
    tstart = buf;
    tend = 0;
    for (i = 0; i < msgy; i++) {
        tend = strchr(tstart, '\n');
        *tend = '\0';

        mydrv->type_msg(tstart,
            rect.x0 + kModalPadding,
            rect.y0 + kModalPadding + 8*i,
            9);

        tstart = (tend + 1);
    }

    // draw the OK button
    mydrv->strokeRect(&ok_rect, 5);
    mydrv->type_msg("Ok",
        ok_rect.x0 + kButtonPadding,
        ok_rect.y0 + kButtonPadding, 
        9);
    
    // finish up
    mydrv->copy_backbuf();
    mydrv->use_backbuf(false);
}
