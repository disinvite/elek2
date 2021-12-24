#include "common/types.h"
#include "input/mouse.h"

#include <dos.h>

int mouse_xpos;
int mouse_ypos;
int mouse_left;
int mouse_right;

#define kMaxMouseEvents 16

int evt_buf[kMaxMouseEvents];
int evt_xpos[kMaxMouseEvents];
int evt_ypos[kMaxMouseEvents];
int evt_write_ptr = 0;
int evt_read_ptr  = 0;

int Mouse_PumpEvents(void) {
    int flags;

    if (evt_read_ptr == evt_write_ptr)
        return 0;

    flags = evt_buf[evt_read_ptr];
    if (flags & kCursorChanged) {
        mouse_xpos = evt_xpos[evt_read_ptr];
        mouse_ypos = evt_ypos[evt_read_ptr];
    }

    if (flags & kLeftPressed)
        mouse_left = 1;

    if (flags & kLeftReleased)
        mouse_left = 0;

    if (flags & kRightPressed)
        mouse_right = 1;

    if (flags & kRightReleased)
        mouse_right = 0;

    evt_read_ptr++;
    evt_read_ptr &= (kMaxMouseEvents - 1);
    return 1;
}
