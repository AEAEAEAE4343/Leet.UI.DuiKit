#include "NotepadWindow.h"
#include "util.h"
#include "NotepadListener.h"
#include "LineNumEditElement.h"
#include <shobjidl_core.h>

using namespace Leet::UI::DuiKit::Notepad;
using namespace DirectUI;

bool NotepadWindow::Create(NotepadWindow** pNotepadWindow, HINSTANCE moduleInstance)
{
    NotepadWindow* pNpWindow = new NotepadWindow();
    Element* pWindowElement = nullptr;
    DUIXmlParser* pParser = nullptr;
    HRESULT hRes = 0;

    // Create native window host
    hRes = NativeHWNDHost::Create((UCString)L"Leet's DUI Notepad", (HWND)NULL, (HICON)NULL, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, (int)WS_OVERLAPPEDWINDOW, 0U, &pNpWindow->_pNativeWindowHost);
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
    pNpWindow->_pParseButton = (Button*)pNpWindow->_pWindowElement->FindDescendent(StrToID(reinterpret_cast<UCString>(L"parsebutton")));
    pNpWindow->_pAutoUpdateCheck = (CCCheckBox*)pNpWindow->_pWindowElement->FindDescendent(StrToID(reinterpret_cast<UCString>(L"autoupdatecheck")));
    pNpWindow->_pStatus = pNpWindow->_pWindowElement->FindDescendent(StrToID(reinterpret_cast<UCString>(L"status")));
    pNpWindow->_pErrorBox = pNpWindow->_pWindowElement->FindDescendent(StrToID(reinterpret_cast<UCString>(L"errorbox")));
    pNpWindow->_pContainer = pNpWindow->_pWindowElement->FindDescendent(StrToID(reinterpret_cast<UCString>(L"container")));
    pNpWindow->_pEdit = pNpWindow->_pWindowElement->FindDescendent(StrToID(reinterpret_cast<UCString>(L"edit")));
    pNpWindow->_pMarkupBox = pNpWindow->_pWindowElement->FindDescendent(StrToID(reinterpret_cast<UCString>(L"markupbox")));
    DUIAssert(pNpWindow->_pStatus && pNpWindow->_pContainer && pNpWindow->_pEdit && pNpWindow->_pMarkupBox, "Error in embedded DUIXML file.");

    pNpWindow->_pErrorBox->SetVisible(false);
    pNpWindow->_pErrorBox->SetLayoutPos(-3); // none

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

COMDLG_FILTERSPEC rgSpec[] =
{
    { L"DUIXML files", L"*.duixml;*.xml"},
    { L"Any files", L"*"},
};

bool memcpy_swapped(void* dst, void* src, size_t count)
{
    // Must be even
    if (count & 1)
        return false;

    for (size_t i = 0; i < count; i += 2)
    {
        ((char*)dst)[i + 0] = ((char*)src)[i + 1];
        ((char*)dst)[i + 1] = ((char*)src)[i + 0];
    }
}

INT_PTR AboutDialogProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG)
    {
        HANDLE image = LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
        HWND hwndStatic = GetDlgItem(hWnd, IDC_STATIC2);
        SendMessage(hwndStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)image);
    }
    if (uMsg == WM_COMMAND)
    {
        if (wParam == IDOK)
        {
            EndDialog(hWnd, TRUE);
            return TRUE;
        }
    }
    return FALSE;
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
        {
            IFileOpenDialog* pfd;

            // CoCreate the dialog object.
            HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&pfd));

            if (!SUCCEEDED(hr))
                return true;

            DWORD dwOptions;
            hr = pfd->GetOptions(&dwOptions);
            if (SUCCEEDED(hr))
            {
                hr = pfd->SetOptions(dwOptions | FOS_STRICTFILETYPES | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST);
            }

            if (!SUCCEEDED(hr))
                return true;

            hr = pfd->SetFileTypes(2, rgSpec);

            if (!SUCCEEDED(hr))
                return true;

            // Show the Open dialog.
            hr = pfd->Show(NULL);

            if (SUCCEEDED(hr))
            {
                // Obtain the result of the user interaction.
                IShellItem* psiResult;
                hr = pfd->GetResult(&psiResult);

                if (SUCCEEDED(hr))
                {
                    //
                    // You can add your own code here to handle the results.
                    //
                    PWSTR pszFilePath = NULL;
                    hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (!SUCCEEDED(hr) || !pszFilePath)
                        return true;

                    HANDLE file = CreateFileW(pszFilePath, FILE_GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                    if (!file)
                        return true;

                    LARGE_INTEGER fileSize;
                    GetFileSizeEx(file, &fileSize);

                    HANDLE hMapSrcFile = CreateFileMappingW(file, NULL, PAGE_READONLY, 0, 0, NULL);
                    if (!hMapSrcFile)
                    {
                        CloseHandle(file);
                        return true;
                    }

                    PBYTE pSrcFile = (PBYTE)MapViewOfFile(hMapSrcFile, FILE_MAP_READ, 0, 0, 0);

                    char* text;
                    // If the second and fourth byte are 0        -> UTF-16 LE without BOM
                    if (pSrcFile[0] && !pSrcFile[1] && pSrcFile[2] && !pSrcFile[3])
                    {
                        text = (char*)malloc(fileSize.QuadPart + 2);
                        memcpy_s(text, fileSize.QuadPart, pSrcFile, fileSize.QuadPart);
                        text[fileSize.QuadPart] = 0;
                        text[fileSize.QuadPart + 1] = 0;
                    }
                    // If the first and third byte are 0          -> UTF-16 BE without BOM
                    else if (!pSrcFile[0] && pSrcFile[1] && !pSrcFile[2] && pSrcFile[3])
                    {
                        text = (char*)malloc(fileSize.QuadPart + 2);
                        memcpy_swapped(text, pSrcFile, fileSize.QuadPart);
                        text[fileSize.QuadPart] = 0;
                        text[fileSize.QuadPart + 1] = 0;
                    }
                    // If there is a BOM 0xFF, 0xFE (LE)          -> UTF-16 LE with BOM
                    else if (pSrcFile[0] == 0xFF && pSrcFile[1] == 0xFE)
                    {
                        text = (char*)malloc(fileSize.QuadPart);
                        memcpy_s(text, fileSize.QuadPart - 2, pSrcFile + 2, fileSize.QuadPart - 2);
                        text[fileSize.QuadPart - 2] = 0;
                        text[fileSize.QuadPart - 1] = 0;
                    }
                    // If there is a BOM 0xFE, 0xFF (BE)          -> UTF-16 BE with BOM
                    else if (pSrcFile[0] == 0xFE && pSrcFile[1] == 0xFF) 
                    {
                        text = (char*)malloc(fileSize.QuadPart);
                        memcpy_swapped(text, pSrcFile + 2, fileSize.QuadPart - 2);
                        text[fileSize.QuadPart - 2] = 0;
                        text[fileSize.QuadPart - 1] = 0;
                    }
                    // If there is a BOM 0xEF, 0xBB, 0xBF         -> UTF-8 with BOM
                    else if (pSrcFile[0] == 0xEF && pSrcFile[1] == 0xBB && pSrcFile[2] == 0xBF)
                    {
                        int sizeReq = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, (PCHAR)(pSrcFile + 3), fileSize.QuadPart - 3, 0, 0);
                        text = (char*)malloc(sizeof(wchar_t) * (sizeReq + 2));
                        MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, (PCHAR)(pSrcFile + 3), fileSize.QuadPart - 3, (LPWSTR)text, sizeReq);
                        text[sizeReq * 2] = 0;
                        text[sizeReq * 2 + 1] = 0;
                    }
                    // Otherwise assume UTF-8 (screw ANSI)
                    else
                    {
                        int sizeReq = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, (PCHAR)pSrcFile, fileSize.QuadPart, 0, 0);
                        text = (char*)malloc(sizeof(wchar_t) * (sizeReq + 2));
                        MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, (PCHAR)pSrcFile, fileSize.QuadPart, (LPWSTR)text, sizeReq);
                        text[sizeReq * 2] = 0;
                        text[sizeReq * 2 + 1] = 0;
                    }

                    _pEdit->SetContentString((UCString)text);
                    free(text);

                    UnmapViewOfFile(pSrcFile);
                    CloseHandle(file);

                    CoTaskMemFree(pszFilePath);
                    psiResult->Release();
                }

            }
            return true;
        }
        case ID_FILE_SAVE:
        {
            IFileSaveDialog* pfd;

            // CoCreate the dialog object.
            HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog,
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&pfd));

            if (!SUCCEEDED(hr))
                return true;

            DWORD dwOptions;
            hr = pfd->GetOptions(&dwOptions);
            if (SUCCEEDED(hr))
            {
                hr = pfd->SetOptions(dwOptions | FOS_STRICTFILETYPES | FOS_FORCEFILESYSTEM);
            }

            if (!SUCCEEDED(hr))
                return true;

            hr = pfd->SetFileTypes(2, rgSpec);

            if (!SUCCEEDED(hr))
                return true;

            // Show the Open dialog.
            hr = pfd->Show(NULL);

            if (SUCCEEDED(hr))
            {
                // Obtain the result of the user interaction.
                IShellItem* psiResult;
                hr = pfd->GetResult(&psiResult);

                if (SUCCEEDED(hr))
                {
                    //
                    // You can add your own code here to handle the results.
                    //
                    PWSTR pszFilePath = NULL;
                    hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (!SUCCEEDED(hr) || !pszFilePath)
                        return true;

                    HANDLE file = CreateFileW(pszFilePath, FILE_GENERIC_WRITE | FILE_GENERIC_READ, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                    if (!file)
                        return true;

                    Value* val = nullptr;
                    const wchar_t* text = (const wchar_t*)_pEdit->GetContentStringAsDisplayed(&val);

                    LARGE_INTEGER fileSize;
                    fileSize.QuadPart = sizeof(wchar_t) * lstrlenW(text);

                    HANDLE hMapDstFile = CreateFileMappingW(file, NULL, PAGE_READWRITE, fileSize.HighPart, fileSize.LowPart, NULL);
                    if (!hMapDstFile)
                    {
                        CloseHandle(file);
                        return true;
                    }

                    PBYTE pDstFile = (PBYTE)MapViewOfFile(hMapDstFile, FILE_MAP_WRITE, 0, 0, 0);
                    memcpy_s(pDstFile, fileSize.QuadPart, text, fileSize.QuadPart);
                    if (val) val->Release();
                    else free((void*)text);

                    UnmapViewOfFile(pDstFile);
                    CloseHandle(file);

                    CoTaskMemFree(pszFilePath);
                    psiResult->Release();
                }

            }
            return true;
        }
        case ID_HELP_ABOUT:
            DialogBoxParamW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDD_DIALOG1), GetWindowHandle(), AboutDialogProcedure, NULL);
            break;
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
                case 'R':
                    if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000))
                        break;
                case VK_F5:     // Refresh
                    Refresh(true);

                    pie->handled = true;
                    return;
                case VK_F13:
                    if (_pAutoUpdateCheck->GetSelected()) Refresh(false);

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
                }
            }
        }
    }
}

void NotepadWindow::OnEvent(Element* elem, Event* pEvent)
{
    if (pEvent->type == DirectUI::Button::Click)
    {
        if (pEvent->target == _pParseButton)
        {
            Refresh(true);
        }
    }
    else if (pEvent->flag == GMF_BUBBLED)
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


void NotepadWindow::Refresh(bool showError)
{
    unsigned long defer;
    _pWindowElement->StartDefer(&defer);

    // Remove all children from container
    _pContainer->DestroyAll(true);

    Value* pv = nullptr;

    // Parse text from Edit control
    LPCWSTR pTextW = reinterpret_cast<LPCWSTR>(((LineNumEditElement*)_pEdit)->GetContentStringAsDisplayed(&pv));

    // Convert to single byte for parser
    if (lstrlenW(pTextW) > 0)
    {
        _szParseError[0] = 0;

        DUIXmlParser* pParser;
        bool showBox = false;
        HRESULT hr = DUIXmlParser::Create(&pParser, NULL, NULL, ParserErrorHandler, &showBox);

        hr = pParser->SetXML(reinterpret_cast<UCString>(pTextW), _modInstance, NULL);
        if (SUCCEEDED(hr))
        {
            Element* pe = nullptr;
            pParser->CreateElement(reinterpret_cast<UCString>(L"main"), NULL, NULL, 0, &pe);
            if (pe)
            {
                DirectUIElementAdd(_pContainer, pe);
            }
        }
        else
        {
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

    if (_szParseError[0])
    {
        _pStatus->SetContentString(reinterpret_cast<UCString>(L"Failed"));
        _pStatus->SetForegroundColor(RGB(128, 0, 0));
        if (showError)
        {
            _pErrorBox->SetContentString(reinterpret_cast<UCString>(_szParseError));
            _pErrorBox->SetVisible(true);
            _pErrorBox->SetLayoutPos(3); // bottom
        }
    }
    else
    {
        _pStatus->SetContentString(reinterpret_cast<UCString>(L"Successful"));
        _pStatus->SetForegroundColor(RGB(0, 128, 0));
        _pErrorBox->SetVisible(false);
        _pErrorBox->SetLayoutPos(-3); // none
    }

    if (pv)
        pv->Release();
    else
        free((LPWSTR)pTextW);

    _pWindowElement->EndDefer(defer);
}


NotepadWindow::NotepadWindow() 
{
    
}