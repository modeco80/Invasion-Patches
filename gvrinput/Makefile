# Makefile

include ../common/rules.mk

# libs 2 link
LINK_LIBS := ~/vs2022/winsdk/lib/kernel32.lib \
	../common/lib/msvcrt.lib

VPATH := src/ ../common/src

OUT := GVRInputRaw.dll

# objs
OBJS := dllcrt0.o ctors.o exit.o \
	gvrinput.o

.PHONY: all clean

all: $(OUT)

clean:
	rm $(OBJS) $(OUT)

$(OUT): $(OBJS)
	$(call LINK_DLL)
	rm GVRInputRaw.lib
