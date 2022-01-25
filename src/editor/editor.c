#include "common/types.h"
#include "editor/editor.h"
#include "editor/update.h"
#include "map/screen.h"

// Helpers

static int checkLayer(int layer) {
    return (layer >= 0) && (layer < 4);
}

static int checkTileXY(byte x, byte y) {
    return (x < 14) && (y < 8);
}

static void TileChanged(byte tile) {
    Screen_TileChanged(tile % 13, tile / 13);
}

// Main API

static int selectLayer(editor_t *ed, int layer) {
    if (!checkLayer(layer))
        return -1;

    ed->edit_layer = layer;
    Update_Register(ed, kLayerEditSelect, 0);
    return 0;
}

// Which value will be drawn
static int selectValue(editor_t *ed, byte value) {
    ed->value_selected = value;
    return 0;
}

// Which tile will be drawn on
static int selectTile(editor_t *ed, byte tile) {
    if (tile > 103)
        return 1;

    if (tile != ed->tile_selected)
        TileChanged(ed->tile_selected);

    ed->tile_selected = tile;
    return 0;
}

static int pencil(editor_t *ed) {
    // Can only edit in normal edit mode.
    if (ed->state != kStateNormal)
        return -1;

    if (current_room[ed->edit_layer][ed->tile_selected] != ed->value_selected) {
        current_room[ed->edit_layer][ed->tile_selected] = ed->value_selected;
        TileChanged(ed->tile_selected);
    }

    Update_Register(ed, kPencil, 0);
    return 0;
}

static int openModal(editor_t *ed, modal_t *modal) {
    // Can't open a new modal if one is already open
    if (ed->state != kStateNormal)
        return 1;

    ed->state = kStateShowModal;
    ed->current_modal = modal;
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
    &selectValue,
    &selectTile,
    &pencil,

    &openModal,
    &closeModal,
};
