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
    if (lbutton) {
        api->selectTile(ed, gy * 13 + gx);
        api->pencil(ed);
    } else {
        api->selectTile(ed, gy * 13 + gx);
    }

    return 0;
}

int Mouse_Event(int x, int y, bool lbutton, bool rbutton) {
    int gx, gy;
    MouseScalePoints(x, y, &gx, &gy);

    // 8px dead zone where there aren't any tiles
    if (x >= 624 || y >= 192)
        return 0;

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

    switch (scancode) {
        case SC_1:
        case SC_2:
        case SC_3:
        case SC_4:
            api->selectLayer(ed, scancode - SC_1);
            break;

        case SC_A:
            api->toggleLayer(ed, 0);
            break;

        case SC_S:
            api->toggleLayer(ed, 1);
            break;

        case SC_D:
            api->toggleLayer(ed, 2);
            break;

        case SC_F:
            api->toggleLayer(ed, 3);
            break;
    }

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

