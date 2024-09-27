# Makefile

include ../common/rules.mk

# libs 2 link
LINK_LIBS := ~/vs2022/winsdk/lib/kernel32.lib \
	../common/lib/msvcrt.lib

VPATH := src/ ../common/src ../common/src/base

OUT := GVRInputRaw.dll

# objs
OBJS := $(SDK_OBJECTS_DLL) \
	gvrinput.o

.PHONY: all clean

all: $(OUT)

clean:
	rm $(OBJS) $(OUT)

$(OUT): $(OBJS)
	$(call LINK_DLL)
	rm GVRInputRaw.lib
