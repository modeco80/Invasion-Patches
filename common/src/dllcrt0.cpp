#define _WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501 // XP
#include <windows.h>

// reference count for the CRT
static int __dllmain_attached = 0;

// minimal crt0 for dll using msvcrt.lib
// probably not that needed but whatever

enum class LilyCrtInitState {
	Uninit,
	Init,
};

constinit static LilyCrtInitState crtInitState = LilyCrtInitState::Uninit;

extern "C" {

	// see ./ctors.cpp
	void __lilycrt_call_init();
	void __lilycrt_call_fini();

	DWORD DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpReserved);

	static BOOL _DllMain_Attach(HANDLE hInstDLL, LPVOID reserved) {
		__dllmain_attached++;
		return TRUE;
	}

	static BOOL _DllMain_Detach(HANDLE hInstDLL, LPVOID reserved) {
		if(__dllmain_attached > 0) {
			__dllmain_attached--;

			// Call global destructors
			if(__dllmain_attached == 0) {
				if(crtInitState == LilyCrtInitState::Init) {
					__lilycrt_call_fini();
					crtInitState = LilyCrtInitState::Uninit;
				}
			}
		}

		return TRUE;
	}


	int WINAPI _DllMainCRTStartup(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved) {
		// Run global ctors once.
		if(dwReason == DLL_PROCESS_ATTACH || DLL_THREAD_ATTACH) {
			if(crtInitState == LilyCrtInitState::Uninit) {
				__lilycrt_call_init();
				crtInitState = LilyCrtInitState::Init;
			}
		}
		

		if(DllMain(hinstDLL, dwReason, lpvReserved) == FALSE) {
			// Fake a detach and early fail.
			if(dwReason == DLL_PROCESS_ATTACH) {
				static_cast<void>(_DllMain_Detach(hinstDLL, lpvReserved));
				return FALSE;
			}
		}

		switch(dwReason) {
			case DLL_PROCESS_ATTACH:
				if(!_DllMain_Attach(hinstDLL, lpvReserved))
					return FALSE;
				break;
			case DLL_PROCESS_DETACH:
				if(!_DllMain_Detach(hinstDLL, lpvReserved))
					return FALSE;
				break;
		}
		return TRUE;
	}

}