#ifndef EDITOR_EDITOR_H
#define EDITOR_EDITOR_H

#include "common/types.h"

typedef enum {
    kStateNormal = 0,
    kStateShowModal,
    kStateModalNormal,
    kStateHideModal,
    kStateChangeRoom
} ed_state_t;

typedef struct {
    char *text;
    void (*fn)(void);
} button_t;

typedef struct {
    char *text;
    button_t *buttons[3]; // 3?
    void (*onClose)(void);
} modal_t;

typedef struct {
    // Editor as state machine.
    // For now, just used to show/hide modal
    ed_state_t state;

    // whether each layer is displayed 
    bool layer_display[4];

    // which layer will be modified
    int edit_layer;

    // which tile to draw
    byte tile_selected;
} editor_t;

typedef struct {
    int (*selectLayer)(editor_t *ed, int layer);
    int (*selectTile)(editor_t *ed, byte tile);
    int (*pencil)(editor_t *ed, byte x, byte y);

    int (*openModal)(editor_t *ed, modal_t *modal);
    int (*closeModal)(editor_t *ed);
} editor_api_t;

extern editor_api_t editor_api;

#endif