// Stub GVRInputRaw DLL. 
// FIXME: Implement DirectInput or something so that this is useful

#define _WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501 // XP
#include <windows.h>

#include "util.hpp"

// this matches layout of the official GVRInputRaw.dll
// but we can store anything after +0x14
struct gvrInputStructure {
	char pad[0x14];

	// 0x18 onwards
	__int64 liTimeNow;
	__int64 liTimeLast;
	__int64 liPerfFrequency;
};

#define EXPORT __declspec(dllexport) __cdecl

extern "C" {
	int EXPORT GVRInputRawInit(HWND hWnd, gvrInputStructure* pInput, char* pszRegistryName) {
		util::OutputDebugF("GVRInputRawInit(hWnd: %p, pInput: %p, %s)", hWnd, pInput, pszRegistryName);

		// fail epic style
		if(!pInput)
			return 1;

		QueryPerformanceFrequency((PLARGE_INTEGER)&pInput->liPerfFrequency);
		QueryPerformanceCounter((PLARGE_INTEGER)&pInput->liTimeNow);
		pInput->liTimeLast = pInput->liTimeNow;

		return 0;
	}

	// possible return values GVRInputRaw itself provides:
	// 0 = ok?
	// 1 = no input vtable or no update function in current input vtable
	// 3 = ptr to input structure is null
	// 4 = input update call failed internally

	int EXPORT GVRInputRawUpdate(gvrInputStructure* pInput) {
		if(!pInput)
			return 3;

		
		QueryPerformanceCounter((PLARGE_INTEGER)&pInput->liTimeNow);

		// in MS
		int delta = (int((pInput->liTimeNow - pInput->liTimeLast) * pInput->liPerfFrequency) / 1000 / 1000);

		if(delta > 1000) {
			// for fun!
			for(int i = 0; i < 0x14; ++i)
				pInput->pad[i] += 1;
			util::OutputDebugF("GVRInputRawUpdate(pInput: %p) delta %d", pInput, delta);
		}


		pInput->liTimeLast = pInput->liTimeNow;

		return 0;
	}

	void EXPORT GVRInputRawCleanup() {
		OutputDebugString("GVRInputRawCleanup()");
	}

	BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved) {
		// The DLL is linked to the application so we can't use PROCESS_ATTACH
		if(dwReason == DLL_PROCESS_ATTACH || dwReason == DLL_THREAD_ATTACH) {
			OutputDebugString("[gvrInput] DLL attached");
		}

		return TRUE;
	}

}