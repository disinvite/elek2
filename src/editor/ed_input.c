// Input controller for the editor
#include "common/types.h"
#include "editor/editor.h"

// Done this way so we can mock the editor API for testing purposes.
static editor_t *ed;
static editor_api_t *api;

void EdControl_Setup(editor_t *_ed, editor_api_t *_api) {
    ed = _ed;
    api = _api;
}

static void MouseScalePoints(int xin, int yin, int *xout, int *yout) {
    *xout = xin / 48;
    *yout = yin / 24;
}

int Mouse_Event(int x, int y, bool lbutton, bool rbutton) {
    int gx, gy;
    MouseScalePoints(x, y, &gx, &gy);

    // switch mode.
    api->pencil(ed, gx, gy);

    return 0;
}

int Keyb_Event(int scancode, bool keydown) {
    return 0;
}
