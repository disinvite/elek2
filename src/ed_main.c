// ELEK2 EDITOR
#include "common/types.h"

#include <mem.h>
#include <stdio.h>

#include "editor/editor.h"
#include "editor/ed_input.h"
#include "editor/update.h"
#include "map/mapfile.h"
#include "map/screen.h"
#include "sprite.h"
#include "input/keyb.h"
#include "input/mouse.h"
#include "ui/modal.h"
#include "ui/msgbar.h"
#include "video/video.h"
#include "video/v_mode13.h"

byte pointer_data[576];
editor_t ed;

bool should_redraw = true;

map_packed_t mapfile;
layer_ptr_t layers;


// No special function when you click it.
button_t btnOkGeneric = {"OK", {0, 0, 0, 0}, 0};

modal_t welcomeMsg = {
    "ELEK2 EDITOR\n\n\nby disinvite.",
    {0, 0, 0, 0},
    {0, 0, 0},
    {&btnOkGeneric, 0, 0},
    0
};

void loadCursor(void) {
    int len;
    FILE *f = fopen("data/point24.bin", "rb");
    fread(pointer_data, 576, 1, f);
    fclose(f);
}

void displayMap(void) {
    int i;

    // Rebuild backbuffer as the basis for drawing this room.
    mydrv->use_backbuf(true);
    mydrv->clear();

    for (i = 0; i < 4; i++) {
        if (ed.layer_display & (1 << i))
            mydrv->draw_plane(i, false);
    }

    // Copy back buffer to offscreen.
    mydrv->copy_backbuf();
    mydrv->use_backbuf(false);
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

void drawTheBox(void) {
    rect_t rect = {80, 30, 240, 170};
    rect_t ok_rect = {100, 130, 140, 150};
    mydrv->use_backbuf(true);
    mydrv->darken(true);
    mydrv->fillRect(&rect, 3);
    mydrv->strokeRect(&rect, 5);
    mydrv->type_msg("hello there", 100, 40, 9);
    mydrv->strokeRect(&ok_rect, 5);
    mydrv->type_msg("Ok", 120, 134, 9);
    mydrv->copy_backbuf();
    mydrv->use_backbuf(false);
}

int main(void) {
    int used_vram;
    int keycode;

    mydrv = &mode13_drv;
    Input_Setup();
    Mouse_Init();
    printf("helo\n");

    mydrv->init();
    V_LoadFont("data/bald8x8.fnt");
    V_LoadPal();
    loadCursor();
    mydrv->clear();
    
    readGGS("data/elek1.ggs", 0);
    readGGS("data/elek2.ggs", 1);
    MapPacked_ReadFromFile(&mapfile, "data/elek.ggc");
    mydrv->load_sprites(getSpriteSlot(0), 0, &used_vram);
    mydrv->load_sprites(getSpriteSlot(1), 1, &used_vram);

    EdControl_Setup(&ed, &editor_api);
    editor_api.selectValue(&ed, 5);
    editor_api.openModal(&ed, &welcomeMsg);
    ed.layer_display = 1;
    
    changeRoom(0);
    MsgBar_Set("stay awhile and listen");

    // Mock the first tick so we have something onscreen to look at
    // TODO: This will go away
    // Change room will redraw
    // We also need a first-run state for the app
    // Unless we just show a splash screen or whatever
    displayMap();
    mydrv->update();

    while (1) {
        // Exit on Q or Escape keypress.
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

        // If any tiles just changed as a result of events
        // Update them on the backbuffer
        // TODO: layer flags hardcoded here
        mydrv->redrawChanged(ed.layer_display);

        // Update anything we need to redraw
        mydrv->drect();

        switch (ed.state) {
            case kStateChangeRoom:
                displayMap();
                ed.state = kStateNormal;
                break;

            case kStateShowModal:
                Modal_Prepare(&welcomeMsg);
                Modal_Draw(&welcomeMsg);
                ed.state = kStateModalNormal;
                break;

            case kStateHideModal:
                displayMap();
                ed.state = kStateNormal;
                break;
        }

        mydrv->outline_tile(ed.tile_selected % 13, ed.tile_selected / 13, 1);

        if (update_latest) {
            MsgBar_Set(update_latest);
            update_latest = 0;
        }

        // Pointer (and other overlays) drawn last.
        // Reasonable to expect these to update every frame.
        mydrv->draw24raw(pointer_data, mouse_xpos / 2, mouse_ypos);
        
        // Save dirty rectangle of pointer position because we need
        // to un-paint it on the next frame.
        DRect_Add(mouse_xpos / 2, mouse_ypos, mouse_xpos / 2 + 24, mouse_ypos + 24);

        MsgBar_Tick();
        MsgBar_Draw();

        // Wait for vblank and write to 0xa000.
        mydrv->update();
    }

    mydrv->shutdown();
    Input_Shutdown();
    Mouse_Shutdown();
    MapPacked_FreeData(&mapfile);
    free_sprites();
    return 0;
}
