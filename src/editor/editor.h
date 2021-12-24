#ifndef EDITOR_EDITOR_H
#define EDITOR_EDITOR_H

#include "common/types.h"

typedef struct {
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
} editor_api_t;

extern editor_api_t editor_api;

#endif