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
 * NotepadListener.h: Listener implemented for NotepadWindow
 *
 */

#pragma once
#include "stdafx.h"
#include "NotepadWindow.h"

using namespace DirectUI;

namespace Leet {
namespace UI {
namespace DuiKit {
namespace Notepad
{
    struct ThumbDragEvent : InputEvent
    {
        SIZE sizeDelta;
    };

    struct KeyboardEvent : InputEvent
    {
        WCHAR ch;
        WORD cRep;
        WORD wFlags;
    };

    class NotepadListener : public IElementListener
    {
    private:
        NotepadWindow* _pWindow;
    public:
        NotepadListener(NotepadWindow* window) 
        {
            _pWindow = window;
            _pWindow->AddListener(this);
        }

        void OnListenerAttach(Element* elem) override { }
        void OnListenerDetach(Element* elem) override { }

        bool OnPropertyChanging(Element* elem, const PropertyInfo* prop, int unk, Value* v1, Value* v2) override {
            return true;
        }

        void OnListenedPropertyChanged(Element* elem, const PropertyInfo* prop, int type, Value* v1, Value* v2) override { }
        void OnListenedEvent(Element* elem, struct Event* pEvent) override
        {
            _pWindow->OnEvent(elem, pEvent);
        }

        void OnListenedInput(Element* elem, struct InputEvent* pie) override
        {
            _pWindow->OnInput(elem, pie);
        }
    };

}}}}