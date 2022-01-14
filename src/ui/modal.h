#ifndef UI_MODAL_H
#define UI_MODAL_H

#include "common/types.h"

typedef struct {
    char *text;
    void (*fn)(void);
} button_t;

typedef struct {
    char *text;
    button_t *buttons[3]; // 3?
    void (*onClose)(void);
} modal_t;

void Modal_Draw(modal_t *modal);

#endif
