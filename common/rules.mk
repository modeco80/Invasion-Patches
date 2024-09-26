CC := clang --target=i686-pc-msvc-windows
CXX := clang++ --target=i686-pc-msvc-windows
RC := llvm-rc
LD := lld-link
RC_INCLUDES := /I ~/vs2022/winsdk/include/um /I ~/vs2022/winsdk/include/shared
SDK_INCLUDES := -isystem ~/vs2022/ucrt/include -isystem ~/vs2022/winsdk/include/shared -isystem ~/vs2022/winsdk/include/um

CCFLAGS := $(SDK_INCLUDES) -D_WIN32_WINNT=0x0501 -Ires -std=c17 -O2
CXXFLAGS := $(SDK_INCLUDES) -fno-exceptions -fno-rtti -D_WIN32_WINNT=0x0501 -Ires -std=c++23 -O2

define LINK_EXE
	$(LD) /nodefaultlib /subsystem:windows,5.1 /out:$@ $(LINK_LIBS) $(OBJS)
endef

define LINK_DLL
	$(LD) /dll /nodefaultlib /subsystem:windows,5.1 /out:$@ $(LINK_LIBS) $(OBJS)
endef

# da rules

%.o: %.c
	$(CC) -c $(CCFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

# opinionated towards llvm-rc, since it's what I use
%.res: res/%.rc
	$(RC) $(RC_INCLUDES) /FO $@ $<