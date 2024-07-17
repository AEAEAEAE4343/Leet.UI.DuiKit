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
 * duinotepad.h: Main Entry point
 *
 */

#include "stdafx.h"

#include "resource.h"
#include <parser.h>

#define DUIAssert(a, b) if (!(a)) { MessageBoxA(NULL, b" The application will exit.", "Fatal Error", MB_OK|MB_ICONERROR); exit(1); }

using namespace DirectUI;

HWNDElement* _pePadFrame;
Edit* _peEdit;
Element* _peContainer;
Element* _peStatus;
Element* _peMarkupBox;
HINSTANCE _hInstance;

WCHAR _szParseError[201];
int _dParseError;
bool showBox = false;

void CALLBACK ParserErrorHandler(LPCWSTR pszError, LPCWSTR pszToken, int dLine)
{
    if (dLine != -1)
        swprintf(_szParseError, L"%s '%s' at line %d", pszError, pszToken, dLine);
    else
        swprintf(_szParseError, L"%s '%s'", pszError, pszToken);

    if (showBox)
        MessageBoxW(NULL, _szParseError, L"DUIXML Parser Error", MB_OK);
}

static __int64 DirectUIElementAdd(DirectUI::Element* Parent, DirectUI::Element* Child)
{
    // Thanks wiktorwiktor12 for pointing me to this!
    // https://github.com/wiktorwiktor12/ConsoleLogonHook/blob/dde28b32488d5bdf27e75bdc2d0152c44788b31b/ConsoleLogonUI/util/util.h#L84
    return (*(__int64(__fastcall**)(DirectUI::Element*, DirectUI::Element**, __int64))(*(uintptr_t*)Parent + 128i64))(
        Parent,
        (DirectUI::Element**)&Child,
        1i64);
}

void RefreshPadFrame()
{
    unsigned long defer;
    _pePadFrame->StartDefer(&defer);

    // Remove all children from container
    _peContainer->DestroyAll(true);

    Value* pv;

    // Parse text from Edit control
    LPCWSTR pTextW = reinterpret_cast<LPCWSTR>(_peEdit->GetContentString(&pv));

    // Convert to single byte for parser
    if (lstrlenW(pTextW) > 0)
    {
        DUIXmlParser* pParser;
        showBox = false;
        HRESULT hr = DUIXmlParser::Create(&pParser, NULL, NULL, reinterpret_cast<DirectUI::ParseErrorCallback>(ParserErrorHandler), NULL);
        
        hr = pParser->SetXML(reinterpret_cast<UCString>(pTextW), _hInstance, NULL);
        if (SUCCEEDED(hr))
        {
            Element* pe = nullptr;
            pParser->CreateElement(reinterpret_cast<UCString>(L"main"), NULL, NULL, 0, &pe);
            if (!pe)
                _peStatus->SetContentString(reinterpret_cast<UCString>(_szParseError));
            else
            {
                DirectUIElementAdd(_peContainer, pe);

                _peStatus->SetContentString(reinterpret_cast<UCString>(L"Parse successful!"));
            }
        }
        else
        {
            _peStatus->SetContentString(reinterpret_cast<UCString>(_szParseError));

            // Position caret where error is
            if (_dParseError != -1)
            {
                int dCharIndex = (int)SendMessageW(_peEdit->GetHWND(), EM_LINEINDEX, _dParseError - 1, 0);

                if (dCharIndex != -1)
                    SendMessageW(_peEdit->GetHWND(), EM_SETSEL, dCharIndex, dCharIndex + 1);
            }

            pParser->Destroy();
        }
    }

    pv->Release();

    _pePadFrame->EndDefer(defer);
}

// Frame initialization
void SetupPadFrame()
{
    // Initialize members
    _peStatus = _pePadFrame->FindDescendent(StrToID(reinterpret_cast<UCString>(L"status")));
    _peContainer = _pePadFrame->FindDescendent(StrToID(reinterpret_cast<UCString>(L"container")));
    _peEdit = (Edit*)_pePadFrame->FindDescendent(StrToID(reinterpret_cast<UCString>(L"edit")));
    _peMarkupBox = _pePadFrame->FindDescendent(StrToID(reinterpret_cast<UCString>(L"markupbox")));

    DUIAssert(_peStatus && _peContainer && _peEdit && _peMarkupBox, "Error in persisted UI file.");

    _peStatus->SetContentString(reinterpret_cast<UCString>(L"Enter UI Markup (F5=Refresh, F6/F7=Font Size)"));
}

namespace DirectUI 
{
    struct ThumbDragEvent : InputEvent
    {
        SIZE sizeDelta;
    };

    struct KeyboardEvent2 : InputEvent
    {
        WCHAR ch;
        WORD cRep;
        WORD wFlags;
    };
}

struct EventListener : public IElementListener 
{
    EventListener() { }

    void OnListenerAttach(Element* elem) override { }
    void OnListenerDetach(Element* elem) override { }

    bool OnPropertyChanging(Element* elem, const PropertyInfo* prop, int unk, Value* v1, Value* v2) override {
        return true;
    }

    void OnListenedPropertyChanged(Element* elem, const PropertyInfo* prop, int type, Value* v1, Value* v2) override { }
    void OnListenedEvent(Element* elem, struct Event* pEvent) override 
    {
        if (pEvent->flag == GMF_BUBBLED)
        {
            if (pEvent->type == Thumb::Drag)
            {
                ThumbDragEvent* ptde = (ThumbDragEvent*)pEvent;

                // Markup box is using a local Width, layout will honor it
                int cx = _peMarkupBox->GetWidth();
                cx += ptde->sizeDelta.cx;

                if (cx < 10)
                    cx = 10;
                else if (cx > elem->GetWidth() - 20)  // Frame is also using local Width for size
                    cx = elem->GetWidth() - 20;

                _peMarkupBox->SetWidth(cx);
            }
        }
    }

    void OnListenedInput(Element* elem, struct InputEvent* pie) override 
    { 
        if (pie->flag == GMF_DIRECT || pie->flag == GMF_BUBBLED)
        {
            if (pie->device == GINPUT_KEYBOARD)
            {
                KeyboardEvent2* pke = (KeyboardEvent2*)pie;

                if (pke->code == 0 /*GKEY_DOWN*/)
                {
                    switch (pke->ch)
                    {
                    case VK_F5:     // Refresh
                        RefreshPadFrame();

                        pie->handled = true;
                        return;

                    case VK_F6:     // Font size down
                    case VK_F7:     // Font size up
                    {
                        // Will be negative (point size character height)
                        int dFS = _peEdit->GetFontSize();

                        if (pke->ch == VK_F6)
                        {
                            dFS += 1;
                            if (dFS > -1)
                                dFS = -1;
                        }
                        else
                            dFS -= 1;

                        _peEdit->SetFontSize(dFS);

                        pie->handled = true;
                        return;
                    }
                    }
                }
            }
        }
    }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(nCmdShow);

    HRESULT hr;

    _hInstance = hInstance;

    NativeHWNDHost* pnhh = NULL;
    DUIXmlParser* pParser = NULL;
    Element* pe = NULL;

    // Initialize DUI within the process
    InitProcessPriv(14, NULL, 0, true);

    // Intialize DUI within the thread
    hr = InitThread(2);
    if (FAILED(hr))
        goto Failure;

    // Register DUI control set
    DirectUI::RegisterAllControls();

    // Create native window host
    NativeHWNDHost::Create((UCString)L"Leet's DUI Notepad", (HWND)NULL, (HICON)NULL, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, (int)WS_OVERLAPPEDWINDOW, 0U, &pnhh);

    // Create a main window element
    unsigned long defer_key;
    HWNDElement::Create(pnhh->GetHWND(), true, 0, NULL, &defer_key, (Element**)&_pePadFrame);

    // Create an XML parser
    showBox = true;
    hr = DUIXmlParser::Create(&pParser, NULL, NULL, reinterpret_cast<DirectUI::ParseErrorCallback>(ParserErrorHandler), NULL);
    if (FAILED(hr))
        goto Failure;

    // Set the XML for the parser
    pParser->SetXMLFromResource((DirectUI::UCString)MAKEINTRESOURCEW(IDR_DUIPADUI), hInstance, hInstance);

    // Create the main UI element
    pParser->CreateElement(reinterpret_cast<UCString>(L"duipad"), _pePadFrame, NULL, NULL, &pe);

    // Delete the parser
    pParser->Destroy();
    pParser = NULL;

    // Setup the window
    SetupPadFrame();

    // Set visible and add event handlers
    EventListener listener;
    pe->AddListener(&listener);
    pe->SetVisible(true);
    pe->EndDefer(defer_key);

    // Show the window
    pnhh->Host(pe);
    pnhh->ShowWindow(SW_SHOW);

    // Create the menu
    HWND hWnd = pnhh->GetHWND();
    HMENU menu = LoadMenuW(hInstance, MAKEINTRESOURCEW(IDR_MENU1));
    SetMenu(hWnd, menu);

    // Message loop
    {
        BOOL bRet;
        MSG msg;
        while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
        {
            if (bRet == -1)
            {
                // Handle the error and possibly exit
            }
            else
            {
                TranslateMessage(&msg);

                if (msg.hwnd == hWnd && msg.message == WM_COMMAND)
                {
                    WORD wmId = LOWORD(msg.wParam);
                    WORD wmEvent = HIWORD(msg.wParam);
                    switch (wmId)
                    {
                    case ID_FILE_OPEN:
                        MessageBoxA(hWnd, "Open", NULL, MB_OK);
                        continue;
                    case ID_FILE_SAVE:
                        MessageBoxA(hWnd, "Save", NULL, MB_OK);
                        continue;

                    }
                }

                DispatchMessage(&msg);
            }
        }
    }

Failure:

    if (pParser)
        pParser->Destroy();

    // DirectUI uninitialize thread
    UnInitThread();
    // DirectUI uninitialize process
    UnInitProcessPriv(NULL);

    return 0;
}
