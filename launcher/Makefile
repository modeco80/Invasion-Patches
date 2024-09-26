# Makefile

CXX := clang++ --target=i686-pc-msvc-windows
RC := llvm-rc
LD := lld-link
RC_INCLUDES := /I ~/vs2022/winsdk/include/um /I ~/vs2022/winsdk/include/shared
SDK_INCLUDES := -isystem ~/vs2022/ucrt/include -isystem ~/vs2022/winsdk/include/shared -isystem ~/vs2022/winsdk/include/um

CXXFLAGS := $(SDK_INCLUDES) -fno-exceptions -fno-rtti -D_WIN32_WINNT=0x0501 -Ires -std=c++23 -O2

# libs to link to (keep this minimal)
LINK_LIBS := ~/vs2022/winsdk/lib/kernel32.lib \
	~/vs2022/winsdk/lib/user32.lib \
	../common/lib/msvcrt.lib

OUT := invlaunch.exe

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

%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.res: res/%.rc
	$(RC) $(RC_INCLUDES) /FO $@ $<

$(OUT): $(OBJS)
	lld-link /nodefaultlib /subsystem:windows,5.1 /out:$@ $(LINK_LIBS) $(OBJS)
