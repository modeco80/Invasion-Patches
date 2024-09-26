// Potentional TODO: Import atexit() logic from NanoC?
#define _WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" void __lilycrt_call_fini();

extern "C" void __lilycrt_exit_fast(int code) {
	// Exit without calling terminators.
	ExitProcess(code);
}

extern "C" void __lilycrt_exit(int code) {
	// TODO: call atexit() functions
	__lilycrt_call_fini();

	// exit
	__lilycrt_exit_fast(code);
}

// Public version, any external code should use this.
extern "C" void lilycrt_exit(int code) {
	__lilycrt_exit(code);
}