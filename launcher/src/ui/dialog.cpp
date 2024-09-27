#include "dialog.hpp"

#include "controls.hpp"

namespace ui::impl {
	INT_PTR CALLBACK Dialog::Dialog_DlgProcImpl(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
		Dialog* pDialog = reinterpret_cast<Dialog*>(GetWindowLongPtrA(hWnd, GWL_USERDATA));

		// If the dialog pointer has been set, redirect to the member function.
		// Otherwise the default dialog proc should be used.
		if(pDialog) {
			return pDialog->DlgProc(uiMsg, wParam, lParam);
		}

		return FALSE;
	}

	void Dialog::RunModeless(HINSTANCE hInstance) {
		Create(hInstance, nullptr);

		Initialize();

		// TODO: Center the window or something.. why does createdialog put it at 0,0 by default

		Show();

		PumpLoop();
	}

	void Dialog::Show() {
		ShowWindow(hWnd, SW_SHOW);
	}

	void Dialog::Hide() {
		ShowWindow(hWnd, SW_HIDE);
	}

	void Dialog::Close() {
		SendMessage(hWnd, WM_CLOSE, 0, 0);

		// Control objects are no longer needed, so we can safely delete them
		for(auto i = 0z; i < controls.Size(); ++i) {
			delete controls[i].control;
			controls[i].control = nullptr;
		}

		// Clear the backing DynamicArray too, just to be nice
		// (though, it likely will be being destructed after this call,
		//  so it's rather pessimestic to do this)
		controls.Clear();
		controls.ShrinkToFit();
	}

	void Dialog::Create(HINSTANCE hInstance, HWND hWndParent) {
		hWnd = CreateDialog(hInstance, name, hWndParent, Dialog_DlgProcImpl);
		SetWindowLongPtrA(hWnd, GWL_USERDATA, (LONG_PTR)this);
	}

	ControlBase* Dialog::FindControl(UINT id) {
		for(auto i = 0z; i < controls.Size(); ++i)
			if(controls[i].id == id)
				return controls[i].control;
		return nullptr;
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

				// Handle IDCANCEL. This allows the dialog to be exited with the Esc key.
				if(controlId == IDCANCEL) {
					Close();
					return TRUE;
				}

				if(auto control = FindControl(controlId)) {
					control->OnCommand(command);
				} else {
					// Not handled, or a control was simply not initalized.
					// Delegate to the default dialogproc.
					return FALSE;
				}

				return TRUE;
			} break;

			case WM_DESTROY: PostQuitMessage(0); break;

			default: return FALSE; break;
		}

		return FALSE;
	}
} // namespace ui::impl
