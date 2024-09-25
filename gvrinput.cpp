// Stub GVRInputRaw DLL. 
// FIXME: Implement DirectInput or something so that this is useful

#define _WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501 // XP
#include <windows.h>

// this matches layout of the official GVRInputRaw.dll
// but we can use any layout that fits in given bytes.
struct gvrInputStructure {
	char pad[0x14];

	// 0x18 onwards
	LARGE_INTEGER tNow;
	LARGE_INTEGER liTimeLast;
	LARGE_INTEGER liPerfFrequency;
};

#define EXPORT __declspec(dllexport) __cdecl

extern "C" {
	int EXPORT GVRInputRawInit(HWND hWnd, gvrInputStructure* pInput, char* pszRegistryName) {
		OutputDebugString("GVRInputRawInit() called");
		return 0;
	}

	// possible return values GVRInputRaw itself provides:
	// 0 = ok?
	// 1 = no input vtable or no update function in current input vtable
	// 3 = ptr to input structure is null
	// 4 = input update call failed internally

	int EXPORT GVRInputRawUpdate(gvrInputStructure* pInput) {
		OutputDebugString("GVRInputRawUpdate() called");
		return 0;
	}

	void EXPORT GVRInputRawCleanup() {
		OutputDebugString("GVRInputRawCleanup() called");
	}

	BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved) {
		if(dwReason == DLL_PROCESS_ATTACH) {
			OutputDebugString("stub gvrinputraw DllMain() initalized");
		}

		return TRUE;
	}

}