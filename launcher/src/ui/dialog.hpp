#pragma once

#include <base/Assert.hpp>
#include <base/DynamicArray.hpp>

#define _WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace ui {
	namespace impl {
	
		// controls.hpp
		struct ControlBase;
		
		/// A dialog.
		struct Dialog {
			/// Constructor. [name] can be MAKEINTRESOURCE(xxx).
			Dialog(LPCTSTR name) : name(name) {}

			void RunModeless(HINSTANCE hInstance);

			void Hide();
			void Show();

			HWND Window() const { return hWnd; }

			virtual void Initialize() = 0;

			void Close();

			/// Returns a (cached) typed control class wrapping a dialog control.
			/// Do not delete this pointer. I will be sad if you do.
			template <class Ctrl>
			Ctrl* GetControl(UINT id) {
				if(auto control = FindControl(id); control == nullptr) {
					auto controlHwnd = GetDlgItem(Window(), id);
					// BASE_ASSERT(controlHwnd == NULL);

					auto obj = new Ctrl(controlHwnd);
					controls.PushBack({ .id = id, .control = (ControlBase*)obj });

					return obj;
				} else {
					return static_cast<Ctrl*>(control);
				}
			}

		   private:
			void Create(HINSTANCE hInstance, HWND hWndParent);

			/// Finds a cached control object. If none exists with the given ID,
			/// returns [nullptr].
			ControlBase* FindControl(UINT id);

			/// Runs a relatively-standard Win32 message loop.
			void PumpLoop();

			/// DlgProc. Simply marshals to the class version
			static INT_PTR CALLBACK Dialog_DlgProcImpl(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

			INT_PTR CALLBACK DlgProc(UINT uiMsg, WPARAM wParam, LPARAM lParam);

			LPCTSTR name;
			HWND hWnd;

			/// Cached control entry.
			struct ControlEntry {
				UINT id;
				ControlBase* control;
			};

			base::DynamicArray<ControlEntry> controls {};
		};

	} // namespace impl


	using impl::Dialog;
} // namespace ui