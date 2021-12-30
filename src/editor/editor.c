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

    if (current_room[ed->edit_layer][y*13 + x] != ed->tile_selected) {
        current_room[ed->edit_layer][y*13 + x] = ed->tile_selected;
        Screen_TileChanged(x, y);
    }
    return 0;
}

static int openModal(editor_t *ed, modal_t *modal) {
    // Can't open a new modal if one is already open
    if (ed->state != kStateNormal)
        return 1;

    ed->state = kStateShowModal;
    return 0;
}

static int closeModal(editor_t *ed) {
    // How can you close what isn't open
    if (ed->state != kStateModalNormal)
        return 1;

    ed->state = kStateHideModal;
    return 0;
}

editor_api_t editor_api = {
    &selectLayer,
    &selectTile,
    &pencil,

    &openModal,
    &closeModal,
};
