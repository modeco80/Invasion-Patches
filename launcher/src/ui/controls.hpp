#pragma once

#define _WIN32_LEAN_AND_MEAN
#include <windows.h>
// FIXME: Implement controls in another source file so we don't need
// to include this here
#include <windowsx.h>

#include "event_slot.hpp"

namespace ui {
	namespace impl {
		/// Base class for all controls
		struct ControlBase {
		   protected:
			friend struct Dialog;

			explicit ControlBase(HWND hwndControl) : hWndControl(hwndControl) {}

			// Add WM_* handlers here. They're executed by Dialog
			// (or whatever)
			virtual void OnCommand(UINT command) {};

			HWND hWndControl;
		};

		struct ComboBox : ControlBase {
			// TODO: EventSlots for changes

			void AddString(const char* item) { ComboBox_AddString(hWndControl, item); }
			void SetSelection(INT index) { ComboBox_SetCurSel(hWndControl, index); }
			int CurrentSelection() { return ComboBox_GetCurSel(hWndControl); }

		   protected:
			friend struct Dialog;
			explicit ComboBox(HWND hwndControl) : ControlBase(hwndControl) {}
		};

		struct Button : ControlBase {
			/// This event is fired when the button is clicked.
			EventSlot<void()> OnClick;

		   protected:
			friend struct Dialog;
			explicit Button(HWND hwndControl) : ControlBase(hwndControl) {}

			virtual void OnCommand(UINT command) override {
				if(command == BN_CLICKED) {
					OnClick();
				}
			}
		};

		struct CheckBox : Button {
			/// This event is fired when the checkbox's check state is changed.
			EventSlot<void(bool /* checked */)> OnCheck;

			bool IsChecked() const {
				if(Button_GetCheck(hWndControl))
					return true;
				return false;
			}

		   protected:
			friend struct Dialog;
			explicit CheckBox(HWND hwndControl) : Button(hwndControl) {}

			virtual void OnCommand(UINT command) override {
				Button::OnCommand(command);

				if(command == BN_CLICKED) {
					OnCheck(IsChecked());
				}
			}
		};
	} // namespace impl

	// Controls TODO
	using impl::Button;
	using impl::CheckBox;
	using impl::ComboBox;

} // namespace ui