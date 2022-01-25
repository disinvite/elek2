#include "editor/update.h"
#include "editor/editor.h"

#include <stdio.h>
#include <string.h>

static char *kStrLayerSelected = "Toggle layer %d.";
static char *kStrEditLayer = "Editing layer %d.";
static char *kStrPencil = "Pencil at %d,%d.";

static char msgbuf[40];
static char *pMsg = &msgbuf[0];

char *update_latest = 0;

void Update_Register(editor_t *ed, editor_update_t upd, int arg0) {
    memset(msgbuf, 0, sizeof(msgbuf));

    switch (upd) {
        case kLayerSelect:
            sprintf(pMsg, kStrLayerSelected, arg0);
            break;

        case kLayerEditSelect:
            sprintf(pMsg, kStrEditLayer, ed->edit_layer);
            break;

        case kPencil:
            sprintf(pMsg, kStrPencil, ed->tile_selected % 13, ed->tile_selected / 13);
            break;
    }

    update_latest = pMsg;
}

