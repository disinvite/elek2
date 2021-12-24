#include "test/tester.h"
#include "editor/editor.h"
#include "map/screen.h"

#include <mem.h>

editor_api_t *ed_func = &editor_api;

// Should plot the selected tile value in the expected position.
int pencil_test(void) {
    editor_t ed;

    // TODO: reset function?
    memset(current_room, 0, kRoomSize);
    ed.edit_layer = 1;
    ed.tile_selected = 0x80;
    
    // Use different values for x and y to help verify that it put the
    // value in the correct spot.
    if (ed_func->pencil(&ed, 3, 5))
        return 1;

    if (current_room[1][5 * 13 + 3] != 0x80)
        return 1;

    return 0;
}

/// Should fail if we try to use the pencil on an invalid X or Y value.
int invalid_pencil(void) {
    editor_t ed;

    ed.edit_layer = 1;
    ed.tile_selected = 0x80;

    // invalid xpos
    if (!ed_func->pencil(&ed, 20, 2))
        return -1;

    // invalid ypos
    // Intentionally choosing a ypos that is inside the range for xpos.
    // If the dimensions of the array change, an alarm should go off here.
    if (!ed_func->pencil(&ed, 2, 12))
        return -1;

    return 0;
}

// should select the layer correctly.
int selectLayer_test(void) {
    editor_t ed;

    if (ed_func->selectLayer(&ed, 1))
        return -1;

    if (ed.edit_layer != 1)
        return -1;

    return 0;
}

// should reject invalid layer.
int invalid_selectLayer(void) {
    editor_t ed;
    ed.edit_layer = 1;

    if (!ed_func->selectLayer(&ed, 5))
        return -1;

    // verify that value is unchanged
    if (ed.edit_layer != 1)
        return -1;
    
    return 0;
}

test_t tests[] = {
    {"pencil update", &pencil_test},
    {"pencil reject invalid xy", &invalid_pencil},
    {"select layer", &selectLayer_test},
    {"select layer reject invalid layer", &invalid_selectLayer},
    {0, 0} // EOL
};

void main(void) {
    testmain(tests);
}
