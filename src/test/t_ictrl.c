#include "test/tester.h"
#include "editor/editor.h"
#include "editor/ed_input.h"
#include "test/mock/m_edit.h"

#include <mem.h>

editor_api_t *ed_func = &mock_editor_api;

int pencil_test(void) {
    editor_t ed;

    EdControl_Setup(&ed, ed_func);
    
    if (Mouse_Event(0, 0, true, false))
        return 1;

    if (mockEditLastCalled != kPencil)
        return 1;

    if (mockEditArgs[0] != 0 && mockEditArgs[1] != 0)
        return 1;

    return 0;
}

test_t tests[] = {
    {"click at 0,0 should use the pencil", &pencil_test},
    {0, 0} // EOL
};

void main(void) {
    testmain(tests);
}
