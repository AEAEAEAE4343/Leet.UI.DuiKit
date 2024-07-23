#include "LineNumEditElement.h"
#include "ClassInfo.h"
#include <new>
#include <exception>

#define GSYNC_RECT 1
#define GSYNC_XFORM 2
#define GSYNC_STYLE 3
#define GSYNC_PARENT 4

#define SGR_MOVE 0x00000001
#define SGR_SIZE 0x00000002
#define SGR_CHANGEMASK (SGR_MOVE | SGR_SIZE)
#define SGR_CONTAINER 0x0000000c

#define FS_None                 0x00000000
#define FS_Italic               0x00000001
#define FS_Underline            0x00000002
#define FS_StrikeOut            0x00000004

#define PI_Specified    2
#define WM_CTLLNESTATICCOLOR (WM_USER + 110)

#define GM_EVENT 32768
#define GM_SYNCADAPTOR GM_EVENT + 8

#define FACILITY_DUSER  FACILITY_ITF
#define DU_S_NOTHANDLED MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_DUSER, 0)
#define DU_S_PARTIAL MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_DUSER, 2)

#define GS_ADAPTOR          0x00004000      // Requires extra notifications to host
DirectUI::IClassInfo* LineNumEditElement::Class = NULL;
typedef struct GMSG_SYNCADAPTOR {
    GMSG baseMsg;
    UINT garbage1;
    UINT garbage2;
    UINT nCode;
};

typedef struct MouseEvent : InputEvent
{
    POINT ptClientPxl;
    BYTE bButton;
    UINT nFlags;
};

typedef struct MouseClickEvent : MouseEvent
{
    UINT cClicks;
};

typedef struct MouseWheelEvent : MouseEvent
{
    short sWheel;
};

const UINT win32MouseMap[7][3] =
{
    // GBUTTON_LEFT         GBUTTON_RIGHT       GBUTTON_MIDDLE
    {  WM_MOUSEMOVE,        WM_MOUSEMOVE,       WM_MOUSEMOVE    },  // GMOUSE_MOVE
    {  WM_LBUTTONDOWN,      WM_RBUTTONDOWN,     WM_MBUTTONDOWN  },  // GMOUSE_DOWN
    {  WM_LBUTTONUP,        WM_RBUTTONUP,       WM_MBUTTONUP    },  // GMOUSE_UP
    {  WM_MOUSEMOVE,        WM_MOUSEMOVE,       WM_MOUSEMOVE    },  // GMOUSE_DRAG
    {  WM_MOUSEHOVER,       WM_MOUSEHOVER,      WM_MOUSEHOVER   },  // GMOUSE_HOVER
    {  WM_MOUSEWHEEL,       WM_MOUSEWHEEL,      WM_MOUSEWHEEL   },  // GMOUSE_WHEEL
};

class LineNumEventListener : public IElementListener 
{
public:
    virtual void OnListenerAttach(class Element* elem) override {}
    virtual void OnListenerDetach(class Element* elem) override {}
    virtual bool OnPropertyChanging(class Element* elem, const struct PropertyInfo* prop, int unk, class Value* before, class Value* after) override 
    {
        return true;
    };
    virtual void OnListenedPropertyChanged(class Element* elem, const struct PropertyInfo* prop, int type, class Value* before, class Value* after) override
    {
        //((LineNumEditElement*)elem)->OnPropertyChanged(prop, type, before, after);
    }
    virtual void OnListenedInput(class Element* elem, struct InputEvent* event) override
    {

    }
    virtual void OnListenedEvent(class Element* elem, struct Event* event) override 
    {

    };
};

HRESULT LineNumEditElement::Register()
{
    return DirectUI::ClassInfo<LineNumEditElement, DirectUI::Element>::Register(GetModuleHandleW(NULL));
}

bool LineNumEditElement::GetKeyFocused()
{
    return true;
}

void LineNumEditElement::SetKeyFocus()
{
    SetFocus(_hWndSink);
    SetFocus(_hWndCtrl);
}

UCString LineNumEditElement::GetContentStringAsDisplayed(Value** val)
{
    if (_hWndCtrl)
    {
        int charCount = GetWindowTextLengthW(_hWndCtrl);
        UString windowText = (UString)malloc(sizeof(wchar_t) * (charCount + 1));
        GetWindowTextW(_hWndCtrl, (LPWSTR)windowText, charCount + 1);
        return windowText;
    }
    return Element::GetContentStringAsDisplayed(val);
}

UINT LineNumEditElement::MessageCallback(GMSG* pMsg)
{
    if (pMsg->hgadMsg == GetDisplayNode())
    {
        switch (pMsg->nMsg)
        {
        case GM_SYNCADAPTOR:
        {
            if (_hWndSink && _hWndCtrl)
            {
                GMSG_SYNCADAPTOR* pMsgSync = (GMSG_SYNCADAPTOR*)pMsg;
                switch (pMsgSync->nCode)
                {
                case GSYNC_RECT:
                case GSYNC_XFORM:
                    //DUITrace("Adaptor RECT sync: <%x>\n", this);
                    SyncRect(SGR_MOVE | SGR_SIZE);
                    return DU_S_PARTIAL;
                case GSYNC_PARENT:
                    SyncParent();
                    return DU_S_PARTIAL;
                }
            }
        }
        }
    }

    return DU_S_NOTHANDLED;
}

void LineNumEditElement::OnInput(InputEvent* event)
{
    if (event->flag == GMF_DIRECT && _oldCtrlProc && !IsDestroyed())  // Handle when direct
    {
        switch (event->device)
        {
        case GINPUT_MOUSE:
        {
            MouseEvent* mouseEvent = (MouseEvent*)event;

            // Check if we can map this message to a HWND message
            if ((mouseEvent->code < GMOUSE_MOVE) || (mouseEvent->code > GMOUSE_WHEEL))
            {
                // TODO: error handling?
                break;
            }

            // Convert the button id to an index
            int iButton;
            if (mouseEvent->bButton == GBUTTON_NONE) break;
            else iButton = mouseEvent->bButton - 1;

            // Check if we can map this button to a HWND mouse button
            if ((iButton < 0) || (iButton > 2))
            {
                // TODO: error handling
                break;
            }

            // Map gadget message into HWND message
            UINT uMsg = win32MouseMap[mouseEvent->code][iButton];
            LPARAM lParam = (LPARAM)POINTTOPOINTS(mouseEvent->ptClientPxl);
            WPARAM wParam = NULL;
            switch (mouseEvent->code)
            {
            case GMOUSE_DOWN:
                if (((MouseClickEvent*)event)->cClicks == 1) {
                    uMsg += (WM_LBUTTONDBLCLK - WM_LBUTTONDOWN);
                }
            case GMOUSE_MOVE:
            case GMOUSE_UP:
            case GMOUSE_HOVER:
            case GMOUSE_DRAG:
                wParam = mouseEvent->nFlags;
                break;
            case GMOUSE_WHEEL:
                wParam = MAKEWPARAM((WORD)mouseEvent->nFlags, (WORD)(short)(((MouseWheelEvent*)event)->sWheel));
                break;
            }

            CallWindowProcW(_oldCtrlProc, _hWndCtrl, uMsg, wParam, lParam);
            event->handled = true;
        }
        }
    }
}

void LineNumEditElement::OnHosted(DirectUI::Element* pNewHost)
{
    // TODO: Make sure new host is HWND, otherwise this won't work
    HWND hwndRoot = ((HWNDElement*)pNewHost)->GetHWND();

    Element::OnHosted(pNewHost);

    if (!_hWndSink)
    {
        WNDCLASSEXW wcex;
        wcex.cbSize = sizeof(wcex);

        // If the class doesn't already exist, create it
        if (!GetClassInfoExW(GetModuleHandleW(NULL), L"LeetCtrlNotifySink", &wcex))
        {
            ZeroMemory(&wcex, sizeof(wcex));

            wcex.cbSize = sizeof(WNDCLASSEX);
            wcex.style = CS_GLOBALCLASS;
            wcex.hInstance = GetModuleHandleW(NULL);
            wcex.hCursor = LoadCursorW(NULL, (LPWSTR)IDC_ARROW);
            wcex.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
            wcex.lpszClassName = L"LeetCtrlNotifySink";

            // We do not need a window procedure as this is just a frame to host the actual control in
            wcex.lpfnWndProc = DefWindowProc;

            if (RegisterClassExW(&wcex) == 0)
                return;
        }

        _hWndSink = CreateWindowExW(0, L"LeetCtrlNotifySink", NULL, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, 0, 0, hwndRoot, NULL, NULL, NULL);
        if (!_hWndSink)
            return;

        SetWindowSubclass(_hWndSink, sinkSubclassProc, (UINT_PTR)_hWndSink, reinterpret_cast<DWORD_PTR>(this));
        
        _hWndCtrl = CreateHWND(_hWndSink);
        if (!_hWndCtrl)
            return;

        _oldCtrlProc = (WNDPROC)GetWindowLongPtrW(_hWndCtrl, GWLP_WNDPROC);
        //SetWindowSubclass(_hWndCtrl, ctrlSubclassProc, (UINT_PTR)_hWndCtrl, reinterpret_cast<DWORD_PTR>(this));

        // Listen for adaptor messages (needed for forwarding input)
        void(*SetGadgetStyle)(HGADGET hgadChange, UINT nNewStyle, UINT nMask) = (void(*)(HGADGET hgadChange, UINT nNewStyle, UINT nMask))GetProcAddress(GetModuleHandleW(L"duser.dll"), "SetGadgetStyle");
        SetGadgetStyle(GetDisplayNode(), GS_ADAPTOR, GS_ADAPTOR);

        SetVisible(true);

        // Synchronize the state of the HWND to the current state of Element
        SyncRect(SGR_MOVE | SGR_SIZE);
        SyncParent();
        SyncFont();
        SyncVisible();
        SyncText();

        AddListener(new LineNumEventListener());
    }
    else
    {
        SetParent(_hWndSink, hwndRoot);
    }
}

#define IsProp(a) pPi == a()
#define PI_Local        1
#define DUIV_UNSET         -1
void LineNumEditElement::OnPropertyChanged(const DirectUI::PropertyInfo* pPi, int index, Value* pOld, Value* pNew)
{
    // Call base
    Element::OnPropertyChanged(pPi, index, pOld, pNew);

    if (_hWndCtrl)
    {
        /*if (IsProp(SizeInLayoutProp))
        {
            // Update size
            _sizeCache = *pNew->GetSize();
            SyncRect(SGR_MOVE | SGR_SIZE, true, nullptr, nullptr);
        } 
        if (IsProp(PosInLayoutProp))
        {
            // Update position
            _posCache = *pNew->GetPoint();
            SyncRect(SGR_MOVE | SGR_SIZE, true, nullptr, nullptr);
        }*/
        if (IsProp(FontFaceProp) || IsProp(FontSizeProp) || IsProp(FontWeightProp) || IsProp(FontStyleProp))
        {
            // Update font being used
            SyncFont();
        }
        else if (IsProp(ContentProp))
        {
            // Relect content change into HWND control
            SyncText();
        }
        else if (IsProp(VisibleProp))
        {
            // Update visible state
            SyncVisible();
        }
        if ((pPi == KeyFocusedProp()) && (index == PI_Local) && (pNew->GetType() != DUIV_UNSET))
        {
            // Element received keyboard focus
            HWND hwndCurFocus = GetFocus();
            if (hwndCurFocus != _hWndCtrl)
            {
                // Control doesn't already have keyboard focus, start the cycle here
                SetFocus(_hWndCtrl);
            }

            // Base will set focus to the display node if needed
        }
    }
}

HRESULT LineNumEditElement::CreateInstance(DirectUI::Element* rootElement, unsigned long* debugVariable, DirectUI::Element** newElement)
{
    int hr = E_OUTOFMEMORY;

    // Using HeapAlloc instead of new() is required as DirectUI::Element::_DisplayNodeCallback calls HeapFree() with the element
    LineNumEditElement* instance = (LineNumEditElement*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LineNumEditElement));

    if (instance != NULL)
    {
        new (instance) LineNumEditElement();
        hr = instance->Initialize(0, rootElement, debugVariable);
        if (SUCCEEDED(hr))
        {
            *newElement = instance;
        }
        else
        {
            if (instance != NULL)
            {
                instance->Destroy(TRUE);
                instance = NULL;
            }
        }
    }
    
    return hr;
}

HWND LineNumEditElement::CreateHWND(HWND hwndParent)
{
    int dwStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE;

    HWND hwndEdit = CreateWindowExW(0, L"LineNumEdit", NULL, dwStyle, 0, 0, GetWidth(), GetHeight(), hwndParent, (HMENU)1, NULL, NULL);
    
    return hwndEdit;
}

void LineNumEditElement::SyncRect(UINT nChangeFlags, bool bForceSync, Value* pNewSize, Value* pNewPoint)
{
    // Get size of gadget in container coordinates
    RECT rcConPxl; 
    
    void(*GetGadgetRect)(HGADGET hgad, RECT * rectPixels, UINT nFlags) = (void(*)(HGADGET, RECT*, UINT))GetProcAddress(GetModuleHandleW(L"duser.dll"), "GetGadgetRect");
    GetGadgetRect(GetDisplayNode(), &rcConPxl, SGR_CONTAINER);

    RECT rcBounds;
    GetWindowRect(_hWndSink, &rcBounds);

    if (!EqualRect(&rcConPxl, &rcBounds) || bForceSync)
    {
        bool destroyed = IsDestroyed();
        bool visible = GetVisible();
        if (!IsDestroyed() || bForceSync)
        {
            // Convert flags from gadget to setwindowpos
            UINT nSwpFlags = SWP_NOACTIVATE | SWP_NOZORDER;
            if (!(nChangeFlags & SGR_MOVE))
                nSwpFlags |= SWP_NOMOVE;

            if (!(nChangeFlags & SGR_SIZE))
                nSwpFlags |= SWP_NOSIZE;

            // Calculate padding
            RECT rcSink = rcConPxl;

            Value* pvRect;

            const RECT* prc = GetBorderThickness(&pvRect);
            rcSink.left += prc->left;
            rcSink.top += prc->top;
            rcSink.right -= prc->right;
            rcSink.bottom -= prc->bottom;
            pvRect->Release();

            prc = GetPadding(&pvRect);
            rcSink.left += prc->left;
            rcSink.top += prc->top;
            rcSink.right -= prc->right;
            rcSink.bottom -= prc->bottom;
            pvRect->Release();

            // Bounds check
            if (rcSink.right < rcSink.left)
                rcSink.right = rcSink.left;

            if (rcSink.bottom < rcSink.top)
                rcSink.bottom = rcSink.top;

            SetWindowPos(_hWndSink, NULL, rcSink.left, rcSink.top, rcSink.right - rcSink.left, rcSink.bottom - rcSink.top, nSwpFlags);

            if (nChangeFlags & SGR_SIZE)
            {
                nSwpFlags |= SWP_NOMOVE;
                SetWindowPos(_hWndCtrl, NULL, 0, 0, rcSink.right - rcSink.left, rcSink.bottom - rcSink.top, nSwpFlags);
            }

            // TODO: implement clipping region
        }
    }

}

void LineNumEditElement::SyncParent()
{
    // this is needed, don't ask why
    SyncRect(SGR_MOVE | SGR_SIZE);
}

void LineNumEditElement::SyncFont()
{
    // TODO: implement font shit
    Value* pvFFace;

    LPCWSTR pszFamily = (LPCWSTR)GetFontFace(&pvFFace);
    int dSize = GetFontSize();
    int dWeight = GetFontWeight();
    int dStyle = GetFontStyle();
    int dAngle = 0;

    // Destroy record first, if exists
    if (_hFont)
    {
        DeleteObject(_hFont);
        _hFont = NULL;
    }

    // Create new font
    LOGFONTW lf;
    ZeroMemory(&lf, sizeof(LOGFONT));

    lf.lfHeight = dSize;
    lf.lfWeight = dWeight;
    lf.lfItalic = (dStyle & FS_Italic) != 0;
    lf.lfUnderline = (dStyle & FS_Underline) != 0;
    lf.lfStrikeOut = (dStyle & FS_StrikeOut) != 0;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfEscapement = dAngle;
    lf.lfOrientation = dAngle;
    wcscpy(lf.lfFaceName, pszFamily);

    // Create
    _hFont = CreateFontIndirectW(&lf);

    pvFFace->Release();

    // Send to control
    SendMessageW(_hWndCtrl, WM_SETFONT, (WPARAM)_hFont, TRUE);
}

void LineNumEditElement::SyncVisible()
{
    if (!IsDestroyed())
    {
        ShowWindow(_hWndSink, GetVisible() ? SW_SHOW : SW_HIDE);
        if (GetVisible())
            SyncRect(SGR_MOVE | SGR_SIZE, true);
    }
}

void LineNumEditElement::SyncText()
{
    if (!IsDestroyed())
    {
        // Hosted HWND content
        int dLen = GetWindowTextLengthW(_hWndCtrl) + 1;  // Including NULL terminator
        LPWSTR pszHWND = (LPWSTR)malloc(dLen * sizeof(WCHAR));
        if (pszHWND)
        {
            // Get HWND content
            GetWindowTextW(_hWndCtrl, pszHWND, dLen);

            // Get Element content
            Value* pvNew;
            LPCWSTR pszNew = (LPCWSTR)GetContentString(&pvNew);
            if (!pszNew)
                pszNew = L"";

            // Compare and update if they are different
            if (wcscmp(pszHWND, pszNew))
                SetWindowTextW(_hWndCtrl, pszNew);

            pvNew->Release();

            free(pszHWND);
        }
    }
}

LRESULT LineNumEditElement::sinkSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    LineNumEditElement* phh = (LineNumEditElement*)dwRefData;

    switch (uMsg)
    {
    /*case WM_COMMAND:
    case WM_NOTIFY:
        LRESULT lRes;
        if (phh->OnNotify(uMsg, wParam, lParam, &lRes))
            return lRes;
        break;*/
    case WM_GETOBJECT:
        return 0;
    case WM_DESTROY:
        phh->_hWndSink = NULL;
        break;
    case WM_CTLLNESTATICCOLOR:
    {
        HDC hdc = (HDC)wParam;
        SetDCBrushColor(hdc, RGB(16, 18, 20));
        SetBkColor(hdc, RGB(16, 18, 20));
        SetTextColor(hdc, RGB(45, 45, 45));
        SetDCPenColor(hdc, RGB(45, 45, 45));
        return (LRESULT)GetStockObject(DC_BRUSH);
    }
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    {
        HDC hdc = (HDC)wParam;
        Value* val;

        DirectUI::Fill foreground = *phh->GetForegroundColor(&val);
        val->Release();

        val = phh->GetValue(BackgroundProp, PI_Specified, NULL);
        auto type = val->GetType();
        switch (type)
        {
            // Sys color index
        case 2:
        {
            int color = val->GetInt();
            COLORREF ref = ColorFromEnumI(color);
            val->Release();

            SetDCBrushColor(hdc, ref & 0x00FFFFFF);
            SetBkColor(hdc, ref & 0x00FFFFFF);
            break;
        }
        // Fill structure
        case 9:
        {
            const DirectUI::Fill* color = val->GetFill();

            SetDCBrushColor(hdc, color->ref.cr & 0x00FFFFFF);
            SetBkColor(hdc, color->ref.cr & 0x00FFFFFF);
            break;
        }
        default:
            wprintf(L"Cannot draw background value type %d!", type);
        }

        SetTextColor(hdc, foreground.ref.cr & 0x00FFFFFF);
        SetDCPenColor(hdc, foreground.ref.cr & 0x00FFFFFF);

        return (LRESULT)GetStockObject(DC_BRUSH);
    }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT LineNumEditElement::ctrlSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    LineNumEditElement* phh = (LineNumEditElement*)dwRefData;
    LRESULT result;
    BOOL(*ForwardGadgetMessage)(HGADGET hgadRoot, UINT nMsg, WPARAM wParam, LPARAM lParam, LRESULT * pr) = (BOOL(*)(HGADGET, UINT, WPARAM, LPARAM, LRESULT*))GetProcAddress(GetModuleHandleW(L"duser.dll"), "ForwardGadgetMessage");;

    switch (uMsg)
    {
    case WM_KEYUP:
    case WM_KEYDOWN:
    case WM_CHAR:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSCHAR:
    case WM_KILLFOCUS:
        if (ForwardGadgetMessage(phh->GetDisplayNode(), uMsg, wParam, lParam, &result))
            return result;
        break;

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    case WM_MOUSEHOVER:
    case WM_MOUSEWHEEL:
    {
        HWND hwndRoot = ::GetParent(phh->_hWndSink);

        POINT ptRoot;
        ptRoot.x = GET_X_LPARAM(lParam);
        ptRoot.y = GET_Y_LPARAM(lParam);
        MapWindowPoints(hWnd, hwndRoot, &ptRoot, 1);

        LPARAM lParamNew = (LPARAM)POINTTOPOINTS(ptRoot);

        if (ForwardGadgetMessage(phh->GetDisplayNode(), uMsg, wParam, lParamNew, &result))
            return result;

        break;
    }

    case WM_SETFOCUS:
        phh->SetKeyFocus();
        break;

    case WM_DESTROY:
        phh->_hWndCtrl = NULL;
        break;

    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
