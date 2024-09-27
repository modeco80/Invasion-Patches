#pragma once

#define _WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace ui {
	namespace impl {

		struct Dialog {
			Dialog(LPCTSTR name) : name(name) {}

			void ShowModeless(HINSTANCE hInstance);

			HWND Window() const { return hWnd; }

			virtual void Initialize() = 0;

			virtual bool OnCommand(UINT uiControl, HWND hWndControl, UINT uiCommand) = 0;

			void Close();

			/// Returns a typed control.
			/// FIXME: How would we deal with event dispatch? Could have a array of CtrlBase or something.
			// template<class Ctrl>
			// Ctrl& GetControl(UINT id) const {
			//   ... make control (if exists, on debug assert, on release simply exit hard)
			//   ... add to internal array of CtrlBase* ptrs
			//   return reference (reference so you don't go trying to delete it like a moron)
			// }

		   private:
			void Create(HINSTANCE hInstance, HWND hWndParent);

			void PumpLoop();

			/// DlgProc for dialogs
			static INT_PTR CALLBACK Dialog_DlgProcImpl(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

			INT_PTR CALLBACK DlgProc(UINT uiMsg, WPARAM wParam, LPARAM lParam);

			LPCTSTR name;
			HWND hWnd;
		};

	} // namespace impl

	using impl::Dialog;
} // namespace ui