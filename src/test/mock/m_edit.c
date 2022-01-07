// Mock editor API for testing.
#include "common/types.h"
#include "editor/editor.h"
#include "test/mock/m_edit.h"

#include <mem.h>

// Each mock function has the `argsused` pragma to suppress compiler warnings.
// We intentionally ignore some args, but they have to be there.

editfn_t mockEditLastCalled = 0;
int mockEditArgs[10] = {0};

// call in setup or teardown method of each test.
void MockEditReset(void) {
    mockEditLastCalled = 0;
    memset(mockEditArgs, 0, sizeof(mockEditArgs));
}

#pragma argsused
static int selectLayer(editor_t *ed, int layer) {
    mockEditLastCalled = kSelectLayer;
    mockEditArgs[0] = layer;
    return 0;
}

#pragma argsused
static int selectValue(editor_t *ed, byte value) {
    mockEditLastCalled = kSelectValue;
    mockEditArgs[0] = value;
    return 0;
}

#pragma argsused
static int selectTile(editor_t *ed, byte tile) {
    mockEditLastCalled = kSelectTile;
    mockEditArgs[0] = tile;
    return 0;
}

#pragma argsused
static int pencil(editor_t *ed) {
    mockEditLastCalled = kPencil;
    return 0;
}

#pragma argsused
static int openModal(editor_t *ed, modal_t *modal) {
    mockEditLastCalled = kShowModal;
    // Not useful to save arg to a pointer
    return 0;
}

#pragma argsused
static int closeModal(editor_t *ed) {
    mockEditLastCalled = kCloseModal;
    return 0;
}

editor_api_t mock_editor_api = {
    &selectLayer,
    &selectValue,
    &selectTile,
    &pencil,

    &openModal,
    &closeModal
};