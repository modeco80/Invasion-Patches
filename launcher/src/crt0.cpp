// Mini "CRT0".
//
// This is the first code Windows calls into when the EXE
// is launched.

#define _WIN32_LEAN_AND_MEAN
#include <windows.h>

// Import crt private apis, used to initalize,
// bring down, and exit everything
extern "C" void __lilycrt_call_init();
extern "C" void __lilycrt_call_fini();
extern "C" void __lilycrt_exit(int code);

// Main symbol (this is expected to exist!)
extern "C" int lily_main(HINSTANCE inst);

// Avoids calling windows APIs
extern "C" void *__ImageBase;

namespace {
	// helper to bind the CRT lifetime to C++ object scope.
	// Also automatically calls the provided function as a template arg

	template<auto function>
	struct CrtInitializerAndMainExecutor {

		CrtInitializerAndMainExecutor() {
			__lilycrt_call_init();
			res = function((HINSTANCE)&__ImageBase);
		}

#pragma warning(push)
#pragma warning(disable: 4722) // The end of this destructor signals the end of program life-time, so it ending the program is OK.

		~CrtInitializerAndMainExecutor() {
			__lilycrt_call_fini();
			__lilycrt_exit(res);
		}

#pragma warning(pop)
	private:
		int res;
	};
}

void __cdecl WinMainCRTStartup() {
	CrtInitializerAndMainExecutor<lily_main> executor;
}