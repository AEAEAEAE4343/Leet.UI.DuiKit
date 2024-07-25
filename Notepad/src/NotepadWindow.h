/*
 * Leet.UI.DuiKit.NotePad
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the Simple Classic Theme License as published by Anis
 * Errais (Leet), either version 1.0 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the Simple Classic Theme License along
 * with this program. If not, see <https://simpleclassictheme.nl/license.html>.
 *
 * NotepadWindow.h/NotepadWindow.cpp: Main window
 *
 */

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

		void Refresh(bool showError);
	private:
		NotepadWindow();

		bool ctrlPressed = false;
		HINSTANCE _modInstance;
		DirectUI::NativeHWNDHost* _pNativeWindowHost = nullptr;
		DirectUI::Button* _pParseButton;
		DirectUI::CCCheckBox* _pAutoUpdateCheck;
		DirectUI::Element* _pWindowElement;
		DirectUI::Element* _pStatus;
		DirectUI::Element* _pErrorBox;
		DirectUI::Element* _pContainer;
		DirectUI::Element* _pEdit;
		DirectUI::Element* _pMarkupBox;
	};
}}}}