#ifndef TEST_MOCK_M_EDIT_H
#define TEST_MOCK_M_EDIT_H

#include "common/types.h"
#include "editor/editor.h"

typedef enum {
    kSelectLayer = 1,
    kToggleLayer,
    kSelectValue,
    kSelectTile,
    kPencil,
    kShowModal,
    kCloseModal
} editfn_t;

void MockEditReset(void);

extern editor_api_t mock_editor_api;
extern editfn_t mockEditLastCalled;
extern int mockEditArgs[10];

#endif
