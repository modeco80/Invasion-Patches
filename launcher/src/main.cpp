// clang-format off
// (it will try to reorder these headers in a way which doesn't work)
#include "ui/dialog.hpp"
#define _WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
// clang-format on

#include "patcher.hpp"
#include "resource.h"

// from msvcrt
extern "C" int __cdecl sprintf(char* buffer, const char* format, ...);

template <class T, auto N>
consteval auto ArraySize(T (&array)[N]) -> decltype(sizeof(int)) {
	return N;
}

HINSTANCE gHInstance;

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

struct LauncherApp : ui::Dialog {
	LauncherApp() : ui::Dialog(MAKEINTRESOURCE(IDI_MAIN)) {}

	void Initialize() override {
		auto hWnd = Window();

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

	bool OnCommand(UINT uiControl, HWND hWndControl, UINT uiCommand) override {
		auto hWnd = Window();

		switch(uiControl) {
			case IDC_MAIN_FULLSCREEN: {
				if(IsDlgButtonChecked(hWnd, uiControl) == TRUE) {
					CheckDlgButton(hWnd, uiControl, BST_CHECKED);
				} else {
					CheckDlgButton(hWnd, uiControl, BST_UNCHECKED);
				}
			} break;

			case IDC_MAIN_LAUNCH:
				Launch();
				return true;
				break;

			case IDC_MAIN_EXIT:
				Close();
				return true;
				break;

			default: break;
		}

		return false;
	}

	void Launch() {
		auto hWnd = Window();

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
			case PatchResult::PatchNotInvasion:
				MessageBox(hWnd, "\"Invasion.exe\" is not Invasion Earth.", "Launcher error", MB_OK | MB_ICONHAND);
				break;

			// FIXME: specific errors
			case PatchResult::PatchFailedGeneric:
				MessageBox(hWnd, "The patcher failed during operation.", "Launcher error", MB_OK | MB_ICONHAND);
				break;
		}
	}
};

extern "C" int lily_main(HINSTANCE hInstance) {
	gHInstance = hInstance;

	// Init comctl32
	INITCOMMONCONTROLSEX ctrls { .dwSize = sizeof(ctrls), .dwICC = ICC_WIN95_CLASSES };
	InitCommonControlsEx(&ctrls);

	LauncherApp dialog;
	dialog.ShowModeless(gHInstance);
	return 0;
}