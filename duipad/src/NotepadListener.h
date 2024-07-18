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