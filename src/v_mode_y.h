#ifndef V_MODE_Y_H
#define V_MODE_Y_H

#include "video.h"

// VGA sequence controller
#define SC_INDEX            0x3c4
#define SC_DATA             0x3c5

#define SC_MAP_MASK         0x02
#define SC_MEMORY_MODE      0x04

// VGA graphic controller
#define GC_INDEX            0x3ce
#define GC_DATA             0x3cf

#define GC_READ_MAP         4
#define GC_MODE             5
#define GC_MISC             6
#define GC_BIT_MASK         8

// VGA CRT controller
#define CRTC_INDEX          0x3d4
#define CRTC_DATA           0x3d5

#define CRTC_UNDERLINE      20
#define CRTC_MODE           23

typedef unsigned char byte;

extern video_drv_t mode_y_drv;

#endif
