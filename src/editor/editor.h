#ifndef EDITOR_EDITOR_H
#define EDITOR_EDITOR_H

#include "common/types.h"
#include "ui/modal.h"

typedef enum {
    kStateNormal = 0,
    kStateShowModal,
    kStateModalNormal,
    kStateHideModal,
    kStateChangeRoom
} ed_state_t;

typedef struct {
    // Editor as state machine.
    // For now, just used to show/hide modal
    ed_state_t state;

    // If we are showing/hiding/displaying a modal, this is the one.
    // In any other state this can be NULL
    modal_t *current_modal;

    // whether each layer is displayed 
    bool layer_display[4];

    // which layer will be modified
    int edit_layer;

    // which tile to draw on
    byte tile_selected;

    // which value to draw
    byte value_selected;
} editor_t;

typedef struct {
    int (*selectLayer)(editor_t *ed, int layer);
    int (*selectValue)(editor_t *ed, byte value);
    int (*selectTile)(editor_t *ed, byte tile);
    int (*pencil)(editor_t *ed);

    int (*openModal)(editor_t *ed, modal_t *modal);
    int (*closeModal)(editor_t *ed);
} editor_api_t;

extern editor_api_t editor_api;

#endif