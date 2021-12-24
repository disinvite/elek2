// ELEK2 EDITOR
#include "common/types.h"

#include <conio.h>
#include <stdio.h>

#include "input/mouse.h"
#include "video/video.h"
#include "video/v_mode13.h"

video_drv_t *mydrv = &mode13_drv;

byte pointer_data[576];

void loadPal(void) {
    color_t pal[256];

    FILE *f = fopen("data/em.pal", "rb");
    fread(&pal, 3, 256, f);
    fclose(f);

    mydrv->update_palette(pal);
}

void loadCursor(void) {
    int len;
    FILE *f = fopen("data/point24.bin", "rb");
    fread(pointer_data, 576, 1, f);
    fclose(f);
}

int main(void) {
    Mouse_Init();
    printf("helo\n");

    mydrv->init();
    loadPal();
    loadCursor();
    mydrv->clear();

    while (!kbhit()) {
        while (Mouse_PumpEvents()) {
            /*
            printf("%d %d (%c|%c)\n", mouse_xpos, mouse_ypos,
                mouse_left  ? '.' : ' ',
                mouse_right ? '.' : ' ');
            */
        }
        mydrv->clear();
        mydrv->draw24raw(pointer_data, mouse_xpos / 2, mouse_ypos);
        mydrv->update();
    }

    mydrv->shutdown();
    Mouse_Shutdown();
    return 0;
}
