#define _WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "patcher.hpp"
#include "resource.h"

// from msvcrt
extern "C" int __cdecl sprintf(char* buffer, const char* format, ...);

template <class T, auto N>
consteval auto ArraySize(T (&array)[N]) -> decltype(sizeof(int)) {
	return N;
}

HINSTANCE gHInstance;
HWND hWnd;

void InitDialog();

void Launch();

// for the dropdown and such.
struct ResolutionEntry {
	UINT dwWidth;
	UINT dwHeight;
};

constinit static ResolutionEntry RESOLUTION_ENTRIES[] = {
	// non-widescreen resolutions
	{ 640, 480 },
	{ 800, 600 },
	{ 1024, 768 },
	{ 1152, 864 },
	{ 1280, 960 },
	{ 1400, 1050 },
	{ 1440, 1080 },
	{ 1600, 1200 },

	// widescreen
	{ 1280, 720 },
	{ 1366, 768 },
	{ 1600, 900 },
	{ 1920, 1080 },
	{ 2560, 1600 },
	{ 3840, 2160 }
};

BOOL OnCommand(UINT controlId, UINT command) {
	switch(controlId) {
		case IDC_MAIN_FULLSCREEN: {
			if(IsDlgButtonChecked(hWnd, controlId) == TRUE) {
				CheckDlgButton(hWnd, controlId, BST_CHECKED);
			} else {
				CheckDlgButton(hWnd, controlId, BST_UNCHECKED);
			}
		} break;

		case IDC_MAIN_LAUNCH: Launch(); break;

		case IDC_MAIN_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			return TRUE;
			break;

		default: break;
	}

	return FALSE;
}

INT_PTR CALLBACK MainDlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
	switch(uiMsg) {
		case WM_CLOSE:
			EndDialog(hWnd, 0);
			DestroyWindow(hWnd);
			return TRUE;
			break;

		case WM_COMMAND: {
			// Handle IDCANCEL. This allows the launcher to be exited with
			// the Esc key, as well as the "Exit" button handled in OnCommand().
			if(LOWORD(wParam) == IDCANCEL) {
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				return TRUE;
			}

			return OnCommand(LOWORD(wParam), HIWORD(wParam));
		} break;

		case WM_DESTROY: PostQuitMessage(0); break;

		default: return FALSE; break;
	}
	return FALSE;
}

void InitDialog() {
	char resEntryStringTemp[32] {};

	auto comboBox = GetDlgItem(hWnd, IDC_MAIN_RESOLUTION);

	// Create resolution options on the combo box from our table of resolutions
	for(auto i = 0; i < ArraySize(RESOLUTION_ENTRIES); ++i) {
		auto& ri = RESOLUTION_ENTRIES[i];
		sprintf(&resEntryStringTemp[0], "%ux%u", ri.dwWidth, ri.dwHeight);
		SendMessage(comboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)resEntryStringTemp);
	}

	SendMessage(comboBox, CB_SETCURSEL, 0, (LPARAM)0);
}

void Launch() {
	auto comboBox = GetDlgItem(hWnd, IDC_MAIN_RESOLUTION);

	// Get item index. Then use this to index into the res index array
	int ItemIndex = SendMessage(comboBox, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	auto& resEntry = RESOLUTION_ENTRIES[ItemIndex];

	bool fullscreen = false;

	if(IsDlgButtonChecked(hWnd, IDC_MAIN_FULLSCREEN) == TRUE) {
		fullscreen = true;
	}

	// Run the patcher.
	auto res = InvPatch_Patch({ .res = { .fullscreen = fullscreen, .width = resEntry.dwWidth, .height = resEntry.dwHeight } });

	switch(res) {
		case PatchResult::PatchResultOK: {
			// Hide ourselves. We will pop back up when the game exits.
			ShowWindow(hWnd, SW_HIDE);

			STARTUPINFOA si {};
			PROCESS_INFORMATION pi {};

			if(CreateProcessA("invasion-tmp.exe", nullptr, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi) == 0) {
				ShowWindow(hWnd, SW_SHOW);
				MessageBox(hWnd, "Failed to launch patched binary.", "Launcher error", MB_OK | MB_ICONHAND);
				return;
			}

			// Wait for the game's process to exit.
			WaitForSingleObject(pi.hProcess, INFINITE);

			ShowWindow(hWnd, SW_SHOW);
		} break;

		case PatchResult::PatchInvasionMissing:
			MessageBox(hWnd, "\"Invasion.exe\" is missing. Please check where the launcher is placed.", "Launcher error", MB_OK | MB_ICONHAND);
			break;

		// This is currently dead code
		case PatchResult::PatchNotInvasion: MessageBox(hWnd, "\"Invasion.exe\" is not Invasion Earth.", "Launcher error", MB_OK | MB_ICONHAND); break;

		// FIXME: specific errors
		case PatchResult::PatchFailedGeneric: MessageBox(hWnd, "The patcher failed during operation.", "Launcher error", MB_OK | MB_ICONHAND); break;
	}
}

extern "C" int lily_main(HINSTANCE hInstance) {
	gHInstance = hInstance;

	// show my beautiful handiwork
	hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDI_MAIN), nullptr, MainDlgProc);
	// TODO: Center the window or something why the hell does createdialog put it at 0,0 by default

	ShowWindow(hWnd, SW_SHOW);

	InitDialog();

	// win32 pump and dump
	BOOL ret {};
	MSG msg {};

	while((ret = GetMessage(&msg, nullptr, 0, 0)) != 0) {
		if(ret == -1) {
			// something went explosion so break out of the loop and just exit
			// (peak error handling strategy)
			break;
		} else if(!IsWindow(hWnd) || !IsDialogMessage(hWnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}