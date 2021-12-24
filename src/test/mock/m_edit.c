// Mock editor API for testing.
#include "common/types.h"
#include "editor/editor.h"
#include "test/mock/m_edit.h"

editfn_t mockEditLastCalled = 0;
int mockEditArgs[10] = {0};

static int selectLayer(editor_t *ed, int layer) {
    mockEditLastCalled = kSelectLayer;
    mockEditArgs[0] = layer;
    return 0;
}

static int selectTile(editor_t *ed, byte tile) {
    mockEditLastCalled = kSelectTile;
    mockEditArgs[0] = tile;
    return 0;
}

static int pencil(editor_t *ed, byte x, byte y) {
    mockEditLastCalled = kPencil;
    mockEditArgs[0] = x;
    mockEditArgs[1] = y;
    return 0;
}

editor_api_t mock_editor_api = {
    &selectLayer,
    &selectTile,
    &pencil
};