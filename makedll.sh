
set -xeuo pipefail

compfile() {
	local BASE=$(basename -s '.cpp' $1)
	clang --target=i686-pc-msvc-windows -isystem ~/vs2022/ucrt/include -isystem ~/vs2022/winsdk/include/shared -isystem ~/vs2022/winsdk/include/um -c -O2 $1 -o $BASE.o	
}

# compile files
compfile dllcrt0.cpp
compfile gvrinput.cpp

# link them
LINK_LIBS=(
	~/vs2022/winsdk/lib/kernel32.lib 
	msvcrt.lib
)

lld-link /dll /nodefaultlib /subsystem:windows,5.1 /out:GVRInputRaw.dll \
	 ${LINK_LIBS[@]} \
	dllcrt0.o gvrinput.o

# un-needed
rm GVRInputRaw.lib

cp GVRInputRaw.dll ../

#i686-w64-mingw32-g++ -c -fno-ident -fno-stack-protector -fomit-frame-pointer -fno-unwind-tables -fno-asynchronous-unwind-tables -O2 gvrinput.cpp -o gvrinput.o
#i686-w64-mingw32-gcc -nodefaultlibs -nostartfiles -s -shared -Wl,--subsystem=windows -Wl,--disable-runtime-pseudo-reloc gvrinput.o  GVRInputRaw.dll
