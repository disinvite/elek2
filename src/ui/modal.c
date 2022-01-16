#include "ui/modal.h"

#include "common/string.h"
#include <string.h>

#include "video/video.h"

// padding around the entire window
#define kModalPadding 16

// padding inside/between each button
#define kButtonPadding 8

void Modal_Prepare(modal_t *modal) {
    int msgx, msgy;
    int pixx, pixy;
    int button_xsize;
    int i;
    int btn_temp_x;
    int btn_temp_y;

    // figure out how much space we need for the buttons
    // fencepost principle dictates that we begin with modal padding
    // each button will add its own margin if needed
    button_xsize = kModalPadding;
    for (i = 0; i < kModalMaxButtons; i++) {
        // If there is a gap and we're not at the end of the list
        // Move the buttons together to close the gap.
        if (!modal->buttons[i] && i < (kModalMaxButtons - 1)) {
            modal->buttons[i] = modal->buttons[i+1];
            modal->buttons[i+1] = 0;
        }

        if (modal->buttons[i]) {
            // modal padding between buttons
            // 2x button padding inside buttons
            // 8px for each char
            button_xsize += kModalPadding
                + 2 * kButtonPadding
                + 8 * strlen(modal->buttons[i]->text);
        }
    }

    if (!modal->buttons[0]) {
        // TODO: default to OK button?
    }

    // figure out how big the box should be
    string_getdim(modal->text, &msgx, &msgy);

    // convert char dims to pixel unit.
    pixx = msgx * 8 + 2 * kModalPadding;
    pixy = msgy * 8 + 3 * kModalPadding + 2 * kButtonPadding + 8;

    // Seems unlikely that this would be the case, but OK
    if (button_xsize > pixx)
        pixx = button_xsize;

    // Create the modal rect centered on the 320x200 screen
    // with enough space for everything
    modal->pos.x0 = 160 - (pixx/2);
    modal->pos.x1 = modal->pos.x0 + pixx;
    modal->pos.y0 = 100 - (pixy/2);
    modal->pos.y1 = modal->pos.y0 + pixy;

    // now set up each button.
    // Start placing buttons from the left.
    btn_temp_x = kModalPadding;
    btn_temp_y = pixy - (2*kButtonPadding + 8 + kModalPadding);

    for (i = 0; i < kModalMaxButtons; i++) {
        if (!modal->buttons[i])
            continue;

        modal->buttons[i]->pos.x0 = btn_temp_x;
        modal->buttons[i]->pos.y0 = btn_temp_y;
        modal->buttons[i]->pos.x1 = modal->buttons[i]->pos.x0
            + (8*strlen(modal->buttons[i]->text))
            + (2*kButtonPadding);
        modal->buttons[i]->pos.y1 = modal->buttons[i]->pos.y0 + (2*kButtonPadding + 8);

        btn_temp_x = modal->buttons[i]->pos.x1 + kModalPadding;
    }
}

void Modal_Draw(modal_t *modal) {
    int i;
    rect_t rtemp;
    button_t *tbtn;

    // prepare the stage for drawing the modal
    mydrv->use_backbuf(true);
    mydrv->darken(true);

    // draw the box for the window
    mydrv->fillRect(&modal->pos, 3);
    mydrv->strokeRect(&modal->pos, 5);

    // type main modal text
    mydrv->type_msg(modal->text,
        modal->pos.x0 + kModalPadding,
        modal->pos.y0 + kModalPadding,
        9);

    // draw the OK button
    for (i = 0; i < kModalMaxButtons; i++) {
        tbtn = modal->buttons[i];
        if (!tbtn)
            continue;

        // TODO: definite util function here.
        rtemp.x0 = modal->pos.x0 + tbtn->pos.x0;
        rtemp.x1 = modal->pos.x0 + tbtn->pos.x1;
        rtemp.y0 = modal->pos.y0 + tbtn->pos.y0;
        rtemp.y1 = modal->pos.y0 + tbtn->pos.y1;

        mydrv->strokeRect(&rtemp, 5);
        mydrv->type_msg(tbtn->text,
            modal->pos.x0 + tbtn->pos.x0 + kButtonPadding,
            modal->pos.y0 + tbtn->pos.y0 + kButtonPadding,
            9);
    }
    
    // finish up
    mydrv->copy_backbuf();
    mydrv->use_backbuf(false);
}
