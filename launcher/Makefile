# Makefile

include ../common/rules.mk


# libs to link to (keep this minimal)
LINK_LIBS := ~/vs2022/winsdk/lib/kernel32.lib \
	~/vs2022/winsdk/lib/user32.lib \
	~/vs2022/winsdk/lib/comctl32.lib \
	../common/lib/msvcrt.lib

OUT := invlaunch.exe

VPATH := src/ ../common/src

# objs
OBJS := crt0.o \
	ctors.o \
	exit.o \
	launcher.res \
	patcher.o \
	main.o

.PHONY: all clean

all: $(OUT)

clean:
	rm $(OBJS) $(OUT)

$(OUT): $(OBJS)
	$(call LINK_EXE)
