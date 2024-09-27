#include "dialog.hpp"

namespace ui::impl {
	INT_PTR CALLBACK Dialog::Dialog_DlgProcImpl(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
		Dialog* pDialog = reinterpret_cast<Dialog*>(GetWindowLongPtrA(hWnd, GWL_USERDATA));

		// If the dialog pointer has been set, redirect to the member function.
		// Otherwise the default dialog proc should kick in
		if(pDialog) {
			return pDialog->DlgProc(uiMsg, wParam, lParam);
		}

		return FALSE;
	}

	void Dialog::ShowModeless(HINSTANCE hInstance) {
		Create(hInstance, nullptr);

		Initialize();

		// TODO: Center the window or something.. why does createdialog put it at 0,0 by default
		ShowWindow(hWnd, SW_SHOW);

		PumpLoop();
	}

	void Dialog::Close() {
		SendMessage(hWnd, WM_CLOSE, 0, 0);
	}

	void Dialog::Create(HINSTANCE hInstance, HWND hWndParent) {
		hWnd = CreateDialog(hInstance, name, hWndParent, Dialog_DlgProcImpl);
		SetWindowLongPtrA(hWnd, GWL_USERDATA, (LONG_PTR)this);
	}

	void Dialog::PumpLoop() {
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
	}

	/// DlgProc for dialogs
	static INT_PTR CALLBACK Dialog_DlgProcImpl(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	INT_PTR CALLBACK Dialog::DlgProc(UINT uiMsg, WPARAM wParam, LPARAM lParam) {
		auto hWnd = Window();

		switch(uiMsg) {
			case WM_CLOSE:
				EndDialog(hWnd, 0);
				DestroyWindow(hWnd);
				return TRUE;
				break;

			case WM_COMMAND: {
                auto controlId = LOWORD(wParam);
                auto command = HIWORD(wParam);
				auto controlhWnd = GetDlgItem(hWnd, controlId);

				// Handle IDCANCEL. This allows the dialog to be exited with the Esc key.
				if(controlId == IDCANCEL) {
					Close();
					return TRUE;
				}

				// if OnCommand() returns false then this failed
				// FIXME: Track controls and add typed shims over them. or something
				if(!OnCommand(controlId, controlhWnd, command))
					return FALSE;

				return TRUE;
			} break;

			case WM_DESTROY: PostQuitMessage(0); break;

			default: return FALSE; break;
		}

		return FALSE;
	}
} // namespace ui::impl
