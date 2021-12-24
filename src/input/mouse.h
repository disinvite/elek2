#ifndef INPUT_MOUSE_H
#define INPUT_MOUSE_H

#include "common/types.h"

// Bit masks for mouse events as registered by the interrupt.
// Read bits from AX
#define kCursorChanged   1
#define kLeftPressed     2
#define kLeftReleased    4
#define kRightPressed    8
#define kRightReleased  16

int Mouse_PumpEvents(void);
extern void Mouse_Init(void);
extern void Mouse_Shutdown(void);

extern int mouse_xpos;
extern int mouse_ypos;
extern int mouse_left;
extern int mouse_right;

#endif
