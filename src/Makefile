MODEL = l
DIR_HEADERS = C:\bc31\include
DIR_LIB = C:\bc31\lib
CCFLAGS = -1- -m$(MODEL) -Fc -K -O2
LDFLAGS = /c /s /v /L$(DIR_LIB)
TASMFLAGS = /w2 /ml /o

objs =  \
 main.obj \
 mapfile.obj \
 sprite.obj \
 input.obj

elek2: $(objs)
  TLINK $(LDFLAGS) @&&!
C0$(MODEL).obj $**
!,$@,$@,CL

.c.obj:
  BCC $(CCFLAGS) -L$(DIR_LIB) -I$(DIR_HEADERS) -c $<

.asm.obj:
  TASM $(TASMFLAGS) $<
