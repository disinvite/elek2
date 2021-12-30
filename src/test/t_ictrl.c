#include "common/types.h"
#include "common/scancode.h"
#include "test/tester.h"
#include "editor/editor.h"
#include "editor/ed_input.h"
#include "test/mock/m_edit.h"

#include <mem.h>

editor_api_t *ed_func = &mock_editor_api;

int pencil_test(void) {
    editor_t ed;

    EdControl_Setup(&ed, ed_func);
    ed.state = kStateNormal;
    MockEditReset();

    if (Mouse_Event(0, 0, false, false))
        return 1;

    // should NOT have called pencil method
    if (mockEditLastCalled == kPencil)
        return 1;
    
    MockEditReset();

    if (Mouse_Event(0, 0, true, false))
        return 1;

    if (mockEditLastCalled != kPencil)
        return 1;

    if (mockEditArgs[0] != 0 && mockEditArgs[1] != 0)
        return 1;

    MockEditReset();

    if (Mouse_Event(200, 50, true, false))
        return 1;

    if (mockEditLastCalled != kPencil)
        return 1;

    if (mockEditArgs[0] != 8 && mockEditArgs[1] != 2)
        return 1;

    return 0;
}

int layerselect_test(void) {
    editor_t ed;

    EdControl_Setup(&ed, ed_func);
    ed.state = kStateNormal;

    // 1 key selects first layer (id=0)
    if (Keyb_Event(SC_1, true))
        return 1;

    if (mockEditLastCalled != kSelectLayer || mockEditArgs[0] != 0)
        return 1;

    // 3 key selects third layer (id=2)
    if (Keyb_Event(SC_3, true))
        return 1;

    if (mockEditLastCalled != kSelectLayer || mockEditArgs[0] != 2)
        return 1;

    return 0;
}

test_t tests[] = {
    {"click at 0,0 should use the pencil", &pencil_test},
    {"number keys should select layer", &layerselect_test},
    {0, 0} // EOL
};

void main(void) {
    testmain(tests);
}
