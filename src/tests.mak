MODEL = l
DIR_HEADERS = C:\bc31\include
DIR_LIB = C:\bc31\lib
CCFLAGS = -1- -m$(MODEL) -Fc -K -O2
LDFLAGS = /c /s /v /L$(DIR_LIB)
TASMFLAGS = /w2 /ml /o

t_map_objs =  \
 map\\mapfile.obj \
 test\\tester.obj \
 test\\t_map.obj

t_screen_objs = \
	map\\screen.obj \
	test\\tester.obj \
	test\\t_scr.obj

t_edit_objs = \
	editor\\editor.obj \
	test\\tester.obj \
	test\\t_edit.obj

t_ictrl_objs = \
	editor\\ed_input.obj \
	test\\mock\\m_edit.obj \
	test\\tester.obj \
	test\\t_ictrl.obj

all: tmap tscr tedit tctrl

tmap: $(t_map_objs)
  TLINK $(LDFLAGS) @&&!
C0$(MODEL).obj $**
!,$@,$@,CL

tscr: $(t_screen_objs)
  TLINK $(LDFLAGS) @&&!
C0$(MODEL).obj $**
!,$@,$@,CL

tedit: $(t_edit_objs)
  TLINK $(LDFLAGS) @&&!
C0$(MODEL).obj $**
!,$@,$@,CL

tctrl: $(t_ictrl_objs)
  TLINK $(LDFLAGS) @&&!
C0$(MODEL).obj $**
!,$@,$@,CL

.c.obj:
  BCC $(CCFLAGS) -L$(DIR_LIB) -I$(DIR_HEADERS) -c -o$*.obj $<

.asm.obj:
  TASM $(TASMFLAGS) $<
