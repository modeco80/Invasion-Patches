# Makefile

CXX := clang++ --target=i686-pc-msvc-windows
LD := lld-link
SDK_INCLUDES := -isystem ~/vs2022/ucrt/include -isystem ~/vs2022/winsdk/include/shared -isystem ~/vs2022/winsdk/include/um

CXXFLAGS := $(SDK_INCLUDES) -std=c++23 -O2

# libs 2 link
LINK_LIBS := ~/vs2022/winsdk/lib/kernel32.lib \
	../common/lib/msvcrt.lib

# objs
OBJS := dllcrt0.o \
	gvrinput.o

.PHONY: all clean

all: GVRInputRaw.dll

clean:
	rm $(OBJS) GVRInputRaw.dll

%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

GVRInputRaw.dll: $(OBJS)
	lld-link /dll /nodefaultlib /subsystem:windows,5.1 /out:$@ $(LINK_LIBS) $(OBJS)
	rm GVRInputRaw.lib
