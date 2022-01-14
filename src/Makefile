MODEL = l
DIR_HEADERS = C:\bc31\include
DIR_LIB = C:\bc31\lib
CCFLAGS = -1- -m$(MODEL) -Fc -K -O2
LDFLAGS = /c /s /v /L$(DIR_LIB)
TASMFLAGS = /w2 /ml /o

objs =  \
 main.obj \
 map\\mapfile.obj \
 map\\screen.obj \
 sprite.obj \
 input\\keyb.obj \
 input\\timer.obj \
 dbgcon.obj \
 video\\video.obj \
 video\\v_mode13.obj 

ed_objs = \
	ed_main.obj \
	common\\string.obj \
	editor\\editor.obj \
	editor\\ed_input.obj \
	map\\mapfile.obj \
	map\\screen.obj \
	input\\keyb.obj \
	input\\mouse.obj \
	input\\mouse_a.obj \
	ui\\modal.obj \
	ui\\msgbar.obj \
	video\\video.obj \
	video\\v_mode13.obj \
	sprite.obj

all: elek2 editor

clean:
		del input\\*.obj
		del map\\*.obj
		del video\\*.obj
		del *.obj

elek2: $(objs)
  TLINK $(LDFLAGS) @&&!
C0$(MODEL).obj $**
!,$@,$@,CL

editor: $(ed_objs)
  TLINK $(LDFLAGS) @&&!
C0$(MODEL).obj $**
!,$@,$@,CL

.c.obj:
  BCC $(CCFLAGS) -L$(DIR_LIB) -I$(DIR_HEADERS) -c -o$*.obj $<

.asm.obj:
  TASM $(TASMFLAGS) $< $*.obj
