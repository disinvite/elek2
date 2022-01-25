#ifndef EDITOR_UPDATE_H
#define EDITOR_UPDATE_H

#include "editor/editor.h"

typedef enum {
    kLayerSelect,
    kLayerEditSelect,
    kPencil
} editor_update_t;

extern char *update_latest;

void Update_Register(editor_t *ed, editor_update_t upd, int arg0);

#endif
