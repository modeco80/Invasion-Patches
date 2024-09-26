// Stub GVRInputRaw DLL. 
// FIXME: Implement DirectInput or something so that this is useful

#define _WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501 // XP
#include <windows.h>

#include "util.hpp"

enum GVRRawInputErrorCode {
	GVR_EC_NONE = 0,
	GVR_EC_NOT_INITIALIZED = 1,
	GVR_EC_NO_DEVICE_FOUND = 2,
	GVR_EC_BAD_DATA_POINTER = 3,
	GVR_EC_UPDATE_ERROR = 4
};

// matches layout of the official GVRInputRaw.dll
struct GVRInputRawData {
	short axis_x;
	short axis_y;
	short delta_x;
	short delta_y;
  	short last_x;
	short last_y;
	unsigned long buttons_new;
  	unsigned long buttons_old;

	// 0x18 onwards
	__int64 liTimeNow;
	__int64 liTimeLast;
	__int64 liPerfFrequency;
};

static_assert(sizeof(GVRInputRawData) == 0x30, "doesn't match");

#define GVRINPUT_API __declspec(dllexport) __cdecl

extern "C" {
	int GVRINPUT_API GVRInputRawInit(HWND hWnd, GVRInputRawData* pInput, char* pszRegistryName) {
		util::OutputDebugF("GVRInputRawInit(hWnd: %p, pInput: %p, %s)", hWnd, pInput, pszRegistryName);

		// fail epic style
		if(!pInput)
			return GVR_EC_BAD_DATA_POINTER;

		QueryPerformanceFrequency((PLARGE_INTEGER)&pInput->liPerfFrequency);
		QueryPerformanceCounter((PLARGE_INTEGER)&pInput->liTimeNow);
		pInput->liTimeLast = pInput->liTimeNow;

		return GVR_EC_NONE;
	}

	// possible return values GVRInputRaw itself provides:
	// 0 = ok?
	// 1 = no input vtable or no update function in current input vtable
	// 3 = ptr to input structure is null
	// 4 = input update call failed internally

	GVRRawInputErrorCode GVRINPUT_API GVRInputRawUpdate(GVRInputRawData* pInput) {
		if(!pInput)
			return GVR_EC_BAD_DATA_POINTER;

		
		QueryPerformanceCounter((PLARGE_INTEGER)&pInput->liTimeNow);

		// FIXME: deltatime

		pInput->liTimeLast = pInput->liTimeNow;

		return GVR_EC_NONE;
	}

	void GVRINPUT_API GVRInputRawCleanup() {
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