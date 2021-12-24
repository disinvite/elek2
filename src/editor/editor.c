#include "common/types.h"
#include "editor/editor.h"
#include "map/screen.h"

// Helpers

static int checkLayer(int layer) {
    return (layer >= 0) && (layer < 4);
}

static int checkTileXY(byte x, byte y) {
    return (x < 14) && (y < 8);
}

// Main API

static int selectLayer(editor_t *ed, int layer) {
    if (!checkLayer(layer))
        return -1;

    ed->edit_layer = layer;
    return 0;
}

static int selectTile(editor_t *ed, byte tile) {
    ed->tile_selected = tile;
    return 0;
}

static int pencil(editor_t *ed, byte x, byte y) {
    if (!checkTileXY(x, y))
        return -1;

    current_room[ed->edit_layer][y*13 + x] = ed->tile_selected;
    return 0;
}

editor_api_t editor_api = {
    &selectLayer,
    &selectTile,
    &pencil
};
