#include "common/types.h"

#include <dos.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

#include "dbgcon.h"
#include "input/keyb.h"
#include "input/timer.h"
#include "map/mapfile.h"
#include "map/screen.h"
#include "sprite.h"
#include "video/video.h"
#include "video/v_mode13.h"

byte collide_flag = 0;

bool screen_redraw = true;
bool dbgcon_redraw = true;

// map stuff.
map_packed_t mapfile;
layer_ptr_t layers;

void displaySheet(void) {
    int row;
    int col;

    for (row = 0; row < 8; row++) {
        for (col = 0; col < 8; col++) {
            mydrv->draw24(0, 8*row + col, col, row);
        }
    }
}

void displayMap(void) {
    // Rebuild backbuffer as the basis for drawing this room.
    mydrv->use_backbuf(true);
    mydrv->clear();
    mydrv->draw_plane(0, false);
    mydrv->draw_plane(1, false);
    mydrv->draw_plane(2, false);
    mydrv->draw_plane(3, false);

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

void undraw_debug_region(void) {
    mydrv->draw_region(&current_room[0], 0, 13, 0, 3, 0);
    mydrv->draw_region(&current_room[1], 0, 13, 0, 3, 0);
    mydrv->draw_region(&current_room[2], 0, 13, 0, 3, 0);
    mydrv->draw_region(&current_room[3], 0, 13, 0, 3, 0);
}

int main() {
    int changed = 1;
    int cur_screen = 0;
    char buf[20];
    byte used_vram;

    mydrv = &mode13_drv;
    readGGS("data/elek1.ggs", 0);
    readGGS("data/elek2.ggs", 1);
    MapPacked_ReadFromFile(&mapfile, "data/elek.ggc");

    PIT_Setup();
    Input_Setup();
    mydrv->init();
    V_LoadFont("data/bald8x8.fnt");
    V_LoadPal();
    DbgCon_Init();
    //displaySheet();

    // needs to happen after the video init so VRAM is ready.
    mydrv->load_sprites(getSpriteSlot(0), 0, &used_vram);
    mydrv->load_sprites(getSpriteSlot(1), 1, &used_vram);

    while (1) {
        screen_redraw = false;

        if (keyHeld[0x1] || keyHeld[0x10])
            break;

        if (changed) {
            changeRoom(cur_screen);
            //map_decode(cur_screen);
            sprintf(buf, "screen %02x", cur_screen);
            DbgCon_Insert(buf);
            changed = 0;
            screen_redraw = true;
            dirtyRectWritePtr = 0;
        }

        mydrv->drect();

        dbgcon_redraw = DbgCon_Tick(game_seconds);

        if (screen_redraw)
            displayMap();

        if (dbgcon_redraw)
            DbgCon_Draw();

        //if (collide_flag)
            //mydrv->dbg_draw_solid(collision);

        if (keyDown[0x4b]) {
            // left
            cur_screen = (cur_screen & 240) | ((cur_screen - 1) & 15);
            changed = 1;
        } else if (keyDown[0x4d]) {
            // right
            cur_screen = (cur_screen & 240) | ((cur_screen + 1) & 15);
            changed = 1;
        } else if (keyDown[0x48]) {
            // up
            cur_screen = (cur_screen - 16) & 255;
            changed = 1;
        } else if (keyDown[0x50]) {
            // down
            cur_screen = (cur_screen + 16) & 255;
            changed = 1;
        }

        // C key
        if (keyDown[0x2e])
            collide_flag ^= 1;

        memset(keyDown, 0, 101);
        mydrv->update();
    }

    Input_Shutdown();
    mydrv->shutdown();
    MapPacked_FreeData(&mapfile);
    free_sprites();
    DbgCon_Close();
    PIT_Close();

    return 0;
}
