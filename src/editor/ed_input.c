// Input controller for the editor
#include "common/types.h"
#include "common/scancode.h"
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

// TODO: What are the return values here.
static int MouseNormal(int gx, int gy, bool lbutton, bool rbutton) {
    // TODO: note mouse points are already scaled.
    if (lbutton)
        api->pencil(ed, gx, gy);

    return 0;
}

int Mouse_Event(int x, int y, bool lbutton, bool rbutton) {
    int gx, gy;
    MouseScalePoints(x, y, &gx, &gy);

    // TODO: switch mode.
    switch(ed->state) {
        case kStateNormal:
            return MouseNormal(gx, gy, lbutton, rbutton);
    }

    return 0;
}

static int KeyNormal(int scancode, bool keydown) {
    // TODO: keydown or keyup check
    if (scancode >= SC_1 && scancode <= SC_4)
        api->selectLayer(ed, scancode - SC_1);

    return 0;
}

static int KeyModal(int scancode, bool keydown) {
    // TODO: Enter dismiss popup for now.
    if (scancode == SC_ENTER)
        api->closeModal(ed);

    return 0;
}

int Keyb_Event(int scancode, bool keydown) {
    switch (ed->state) {
        case kStateNormal:
            return KeyNormal(scancode, keydown);

        case kStateModalNormal:
            return KeyModal(scancode, keydown);
    }

    return 0;
}

