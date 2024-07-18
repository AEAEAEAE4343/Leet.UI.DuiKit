#pragma once
#include "stdafx.h"

namespace Leet {
namespace UI {
namespace DuiKit {
namespace Notepad
{
	class NotepadWindow
	{
	public:
		static bool Create(NotepadWindow** pNotepadWindow, HINSTANCE moduleInstance);
		
		void AddListener(DirectUI::IElementListener* listener);
		void SetMenu(HMENU menu);
		bool HandleMenuMessage(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

		HWND GetWindowHandle();

		void OnInput(DirectUI::Element* elem, struct DirectUI::InputEvent* pie);
		void OnEvent(DirectUI::Element* elem, struct DirectUI::Event* pEvent);

		void Refresh();
	private:
		NotepadWindow();

		HINSTANCE _modInstance;
		DirectUI::NativeHWNDHost* _pNativeWindowHost = nullptr;
		DirectUI::Element* _pWindowElement;
		DirectUI::Element* _pStatus;
		DirectUI::Element* _pContainer;
		DirectUI::Edit* _pEdit;
		DirectUI::Element* _pMarkupBox;
	};
}}}}