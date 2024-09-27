// clang-format off
// (it will try to reorder these headers in a way which doesn't work)
#include "ui/controls.hpp"
#define _WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>

#include "ui/dialog.hpp"

#include "patcher.hpp"
#include "resource.h"

// clang-format on

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

		// Nab controls
		resOptionBox = GetControl<ui::ComboBox>(IDC_MAIN_RESOLUTION);
		fullscreenToggle = GetControl<ui::CheckBox>(IDC_MAIN_FULLSCREEN);
		exitButton = GetControl<ui::Button>(IDC_MAIN_EXIT);
		launchButton = GetControl<ui::Button>(IDC_MAIN_LAUNCH);

		// attach signals
		exitButton->OnClick.AttachListener(this, &LauncherApp::Close);
		launchButton->OnClick.AttachListener(this, &LauncherApp::Launch);

		// Make resolution options from table.
		for(auto i = 0; i < ArraySize(RESOLUTION_ENTRIES); ++i) {
			char resEntryStringTemp[32] {};
			const auto& ri = RESOLUTION_ENTRIES[i];
			sprintf(&resEntryStringTemp[0], "%ux%u", ri.dwWidth, ri.dwHeight);
			resOptionBox->AddString(&resEntryStringTemp[0]);
		}

		resOptionBox->SetSelection(0);
	}

	void Launch() {
		auto hRawWindow = Window();
		const auto& resEntry = RESOLUTION_ENTRIES[resOptionBox->CurrentSelection()];
		bool fullscreen = fullscreenToggle->IsChecked();

		// Run the patcher.
		auto res = InvPatch_Patch({ .res = { .fullscreen = fullscreen, .width = resEntry.dwWidth, .height = resEntry.dwHeight } });

		switch(res) {
			case PatchResult::PatchResultOK: {
				// Hide ourselves. We will pop back up when the game exits.
				this->Hide();

				STARTUPINFOA si {};
				PROCESS_INFORMATION pi {};

				if(CreateProcessA("invasion-tmp.exe", nullptr, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi) == 0) {
					ShowWindow(hRawWindow, SW_SHOW);
					MessageBox(hRawWindow, "Failed to launch patched binary.", "Launcher error", MB_OK | MB_ICONHAND);
					return;
				}

				// Wait for the game's process to exit.
				WaitForSingleObject(pi.hProcess, INFINITE);

				this->Show();
			} break;

			case PatchResult::PatchInvasionMissing:
				MessageBox(hRawWindow, "\"Invasion.exe\" is missing. Please check where the launcher is placed.", "Launcher error",
						   MB_OK | MB_ICONHAND);
				break;

			// This is currently dead code
			case PatchResult::PatchNotInvasion:
				MessageBox(hRawWindow, "\"Invasion.exe\" is not Invasion Earth.", "Launcher error", MB_OK | MB_ICONHAND);
				break;

			// FIXME: specific errors
			case PatchResult::PatchFailedGeneric:
				MessageBox(hRawWindow, "The patcher failed during operation.", "Launcher error", MB_OK | MB_ICONHAND);
				break;
		}
	}

	// UI controls
	ui::ComboBox* resOptionBox;
	ui::Button* exitButton;
	ui::Button* launchButton;
	ui::CheckBox* fullscreenToggle;
};

extern "C" int lily_main(HINSTANCE hInstance) {
	gHInstance = hInstance;

	// Init comctl32
	INITCOMMONCONTROLSEX ctrls { .dwSize = sizeof(ctrls), .dwICC = ICC_WIN95_CLASSES };
	InitCommonControlsEx(&ctrls);

	LauncherApp app;
	app.RunModeless(gHInstance);
	return 0;
}