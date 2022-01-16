#ifndef UI_MODAL_H
#define UI_MODAL_H

#include "common/types.h"

#define kModalMaxButtons 3

#define kButtonStateNormal 0
#define kButtonStateHover 1
#define kButtonStateActive 2

typedef struct {
    char *text;
    rect_t pos;
    void (*fn)(void);
} button_t;

typedef struct {
    char *text;
    rect_t pos;
    byte button_state[kModalMaxButtons];
    button_t *buttons[kModalMaxButtons];
    void (*onClose)(void);
} modal_t;

void Modal_Prepare(modal_t *modal);
void Modal_Draw(modal_t *modal);

#endif
