// ELEK2 EDITOR
#include "common/types.h"

#include <mem.h>
#include <stdio.h>

#include "editor/editor.h"
#include "editor/ed_input.h"
#include "map/mapfile.h"
#include "map/screen.h"
#include "sprite.h"
#include "input/keyb.h"
#include "input/mouse.h"
#include "video/video.h"
#include "video/v_mode13.h"

byte pointer_data[576];
editor_t ed;

bool should_redraw = true;

map_packed_t mapfile;
layer_ptr_t layers;

void loadCursor(void) {
    int len;
    FILE *f = fopen("data/point24.bin", "rb");
    fread(pointer_data, 576, 1, f);
    fclose(f);
}

void displayMap(void) {
    mydrv->clear();
    mydrv->draw_plane(0, false);
    mydrv->draw_plane(1, false);
    mydrv->draw_plane(2, false);
    mydrv->draw_plane(3, false);

    // Save off the existing screen data here.
    mydrv->copy_backbuf();
}

void changeRoom(int id) {
    int i;
    
    MapPacked_LoadRoom(&mapfile, id, &layers);
    
    for (i = 0; i < 4; i++) {
        if (layers.layer[i])
            Layer_Unpack(layers.layer[i], current_room[i]);
        else
            memset(current_room[i], 0, kRoomSize);
    }
}

int main(void) {
    int used_vram;
    int keycode;

    mydrv = &mode13_drv;
    Input_Setup();
    Mouse_Init();
    printf("helo\n");

    mydrv->init();
    V_LoadPal();
    loadCursor();
    mydrv->clear();
    
    readGGS("data/elek1.ggs", 0);
    readGGS("data/elek2.ggs", 1);
    MapPacked_ReadFromFile(&mapfile, "data/elek.ggc");
    mydrv->load_sprites(getSpriteSlot(0), 0, &used_vram);
    mydrv->load_sprites(getSpriteSlot(1), 1, &used_vram);

    EdControl_Setup(&ed, &editor_api);
    editor_api.selectTile(&ed, 5);
    changeRoom(0);

    while (1) {
        if (keyHeld[0x1] || keyHeld[0x10])
            break;

        while (Mouse_PumpEvents()) {
            Mouse_Event(mouse_xpos, mouse_ypos, mouse_left, mouse_right);

            // TODO: dumb assumption.
            /*
            if (mouse_left) {
                should_redraw = true;
                dirtyRectWritePtr = 0;
            }
            */
        }

        while (Keyb_PumpEvents(&keycode)) {
            Keyb_Event(keycode & 0x7f, ((keycode >> 7) ^ 1));
        }

        mydrv->redrawChanged();

        if (dirtyRectWritePtr > 0) {
            mydrv->drect();
        }

        if (should_redraw) {
            displayMap();
            should_redraw = false;
        }
        mydrv->draw24raw(pointer_data, mouse_xpos / 2, mouse_ypos);
        DRect_Add(mouse_xpos / 2, mouse_ypos, mouse_xpos / 2 + 24, mouse_ypos + 24);
        mydrv->update();
    }

    mydrv->shutdown();
    Input_Shutdown();
    Mouse_Shutdown();
    MapPacked_FreeData(&mapfile);
    free_sprites();
    return 0;
}
