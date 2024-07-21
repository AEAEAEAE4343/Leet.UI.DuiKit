#include "NotepadWindow.h"
#include "util.h"
#include "NotepadListener.h"
#include "LineNumEditElement.h"

using namespace Leet::UI::DuiKit::Notepad;
using namespace DirectUI;

bool NotepadWindow::Create(NotepadWindow** pNotepadWindow, HINSTANCE moduleInstance)
{
    NotepadWindow* pNpWindow = new NotepadWindow();
    Element* pWindowElement = nullptr;
    DUIXmlParser* pParser = nullptr;
    HRESULT hRes = 0;

    // Create native window host
    hRes = NativeHWNDHost::Create((UCString)L"Leet's DUI Notepad", (HWND)NULL, (HICON)NULL, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, (int)WS_OVERLAPPEDWINDOW, 0U, &pNpWindow->_pNativeWindowHost);
    DUIAssert(hRes == S_OK, "Failed to create native window host.");

    // Create a main window element
    unsigned long defer_key;
    hRes = HWNDElement::Create(pNpWindow->_pNativeWindowHost->GetHWND(), true, 0, NULL, &defer_key, (Element**)&pWindowElement);
    DUIAssert(hRes == S_OK, "Failed to create native window element.");

    // Create an XML parser
    hRes = DUIXmlParser::Create(&pParser, NULL, NULL, ParserErrorHandler, NULL);
    DUIAssert(hRes == S_OK, "Failed to create XML parser.");

    // Set the XML for the parser
    pParser->SetXMLFromResource((DirectUI::UCString)MAKEINTRESOURCEW(IDR_DUIPADUI), moduleInstance, moduleInstance);

    // Create the main UI element
    pParser->CreateElement(reinterpret_cast<UCString>(L"duipad"), pWindowElement, NULL, NULL, &pNpWindow->_pWindowElement);

    // Delete the parser
    pParser->Destroy();
    pParser = NULL;

    // Initialize members
    pNpWindow->_pStatus = pNpWindow->_pWindowElement->FindDescendent(StrToID(reinterpret_cast<UCString>(L"statusbar")));
    pNpWindow->_pContainer = pNpWindow->_pWindowElement->FindDescendent(StrToID(reinterpret_cast<UCString>(L"container")));
    pNpWindow->_pEdit = (Edit*)pNpWindow->_pWindowElement->FindDescendent(StrToID(reinterpret_cast<UCString>(L"edit")));
    pNpWindow->_pMarkupBox = pNpWindow->_pWindowElement->FindDescendent(StrToID(reinterpret_cast<UCString>(L"markupbox")));
    DUIAssert(pNpWindow->_pStatus && pNpWindow->_pContainer && pNpWindow->_pEdit && pNpWindow->_pMarkupBox, "Error in embedded DUIXML file.");

    pNpWindow->_pStatus->SetContentString(reinterpret_cast<UCString>(L"Enter UI Markup (F5=Refresh, F6/F7=Font Size)"));
    pNpWindow->_pWindowElement->EndDefer(defer_key);

    // Set visible and add event handlers
    pNpWindow->_pWindowElement->SetVisible(true);

    // Show the window
    pNpWindow->_pNativeWindowHost->Host(pNpWindow->_pWindowElement);
    pNpWindow->_pNativeWindowHost->ShowWindow(SW_SHOW);
    *pNotepadWindow = pNpWindow;

    return true;
}

void NotepadWindow::AddListener(IElementListener* listener)
{
    _pWindowElement->AddListener(listener);
}

void Leet::UI::DuiKit::Notepad::NotepadWindow::SetMenu(HMENU menu)
{
    ::SetMenu(GetWindowHandle(), menu);
}

bool Leet::UI::DuiKit::Notepad::NotepadWindow::HandleMenuMessage(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    if (hWnd == GetWindowHandle() && message == WM_COMMAND)
    {
        WORD wmId = LOWORD(wParam);
        WORD wmEvent = HIWORD(wParam);
        switch (wmId)
        {
        case ID_FILE_OPEN:
            MessageBoxA(hWnd, "Open", NULL, MB_OK);
            return true;
        case ID_FILE_SAVE:
            MessageBoxA(hWnd, "Save", NULL, MB_OK);
            return true;
        }
    }

    return false;
}

HWND NotepadWindow::GetWindowHandle()
{
    return _pNativeWindowHost->GetHWND();
}

void NotepadWindow::OnInput(Element* elem, InputEvent* pie)
{
    if (pie->flag == GMF_DIRECT || pie->flag == GMF_BUBBLED)
    {
        if (pie->device == GINPUT_KEYBOARD)
        {
            KeyboardEvent* pke = (KeyboardEvent*)pie;

            if (pke->code == 0 /*GKEY_DOWN*/)
            {
                switch (pke->ch)
                {
                case VK_F5:     // Refresh
                    Refresh();

                    pie->handled = true;
                    return;

                case VK_F6:     // Font size down
                case VK_F7:     // Font size up
                {
                    // Will be negative (point size character height)
                    int dFS = _pEdit->GetFontSize();

                    if (pke->ch == VK_F6)
                    {
                        dFS += 1;
                        if (dFS > -1)
                            dFS = -1;
                    }
                    else
                        dFS -= 1;

                    _pEdit->SetFontSize(dFS);

                    pie->handled = true;
                    return;
                }
                case VK_F8:
                    _pEdit->SetVisible(true);
                    if (_pEdit->GetVisible())
                        MessageBoxW(NULL, L"VISIBLE", NULL, MB_OK);
                    else
                        MessageBoxW(NULL, L"NOT VISIBLE", NULL, MB_OK);
                    ((LineNumEditElement*)_pEdit)->SyncVisible();
                    ((LineNumEditElement*)_pEdit)->SyncRect(1 | 2, true);
                    break;
                }
            }
        }
    }
}

void NotepadWindow::OnEvent(Element* elem, Event* pEvent)
{
    if (pEvent->flag == GMF_BUBBLED)
    {
        if (pEvent->type == Thumb::Drag)
        {
            ThumbDragEvent* ptde = (ThumbDragEvent*)pEvent;

            // Markup box is using a local Width, layout will honor it
            int cx = _pMarkupBox->GetWidth();
            cx += ptde->sizeDelta.cx;

            if (cx < 10)
                cx = 10;
            else if (cx > elem->GetWidth() - 20)  // Frame is also using local Width for size
                cx = elem->GetWidth() - 20;

            _pMarkupBox->SetWidth(cx);
        }
    }
}


void NotepadWindow::Refresh()
{
    unsigned long defer;
    _pWindowElement->StartDefer(&defer);

    // Remove all children from container
    _pContainer->DestroyAll(true);

    Value* pv;

    // Parse text from Edit control
    LPCWSTR pTextW = reinterpret_cast<LPCWSTR>(_pEdit->GetContentString(&pv));

    // Convert to single byte for parser
    if (lstrlenW(pTextW) > 0)
    {
        DUIXmlParser* pParser;
        bool showBox = false;
        HRESULT hr = DUIXmlParser::Create(&pParser, NULL, NULL, ParserErrorHandler, &showBox);

        hr = pParser->SetXML(reinterpret_cast<UCString>(pTextW), _modInstance, NULL);
        if (SUCCEEDED(hr))
        {
            Element* pe = nullptr;
            pParser->CreateElement(reinterpret_cast<UCString>(L"main"), NULL, NULL, 0, &pe);
            if (!pe)
                _pStatus->SetContentString(reinterpret_cast<UCString>(_szParseError));
            else
            {
                DirectUIElementAdd(_pContainer, pe);

                _pStatus->SetContentString(reinterpret_cast<UCString>(L"Parse successful!"));
            }
        }
        else
        {
            _pStatus->SetContentString(reinterpret_cast<UCString>(_szParseError));

            /*// Position caret where error is
            if (_dParseError != -1)
            {
                int dCharIndex = (int)SendMessageW(_pEdit->GetHWND(), EM_LINEINDEX, _dParseError - 1, 0);

                if (dCharIndex != -1)
                    SendMessageW(_pEdit->GetHWND(), EM_SETSEL, dCharIndex, dCharIndex + 1);
            }*/

            pParser->Destroy();
        }
    }

    pv->Release();

    _pWindowElement->EndDefer(defer);
}


NotepadWindow::NotepadWindow() 
{
    
}