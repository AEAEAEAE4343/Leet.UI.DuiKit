// LineNumEdit.hpp --- textbox with line numbers

#ifndef _INC_WINDOWS
    #include <windows.h>
#endif
#ifndef _INC_WINDOWSX
    #include <windowsx.h>
#endif

// messages for LineNumEdit
#define LNEM_SETLINENUMFORMAT (WM_USER + 100)
#define LNEM_SETNUMOFDIGITS (WM_USER + 101)
#define LNEM_SETLINEMARK (WM_USER + 102)
#define LNEM_CLEARLINEMARKS (WM_USER + 103)
#define LNEM_SETLINEDELTA (WM_USER + 104)
#define LNEM_SETCOLUMNWIDTH (WM_USER + 105)
#define LNEM_GETCOLUMNWIDTH (WM_USER + 106)
#define LNEM_GETLINEMARK (WM_USER + 107)

#define WM_CTLLNESTATICCOLOR (WM_USER + 110)

#ifndef LINENUMEDIT_DEFAULT_DIGITS
    #define LINENUMEDIT_DEFAULT_DIGITS 4
#endif

#ifdef LINENUMEDIT_IMPL // implementation detail

#include <shlwapi.h>
#include <strsafe.h>
#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////
// LineNumBase --- the base class

class LineNumBase
{
public:
    operator HWND() const
    {
        return m_hwnd;
    }

    LineNumBase(HWND hwnd = NULL) : m_hwnd(NULL), m_fnOldWndProc(NULL)
    {
        if (hwnd)
            Attach(hwnd);
    }

    virtual ~LineNumBase()
    {
    }

    virtual LRESULT CALLBACK
    WindowProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    static LRESULT CALLBACK
    WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LineNumBase *pBase =
            reinterpret_cast<LineNumBase*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (pBase)
        {
            LRESULT ret = pBase->WindowProcDx(hwnd, uMsg, wParam, lParam);
            if (uMsg == WM_NCDESTROY)
                pBase->m_hwnd = NULL;
            return ret;
        }
        return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    LRESULT CALLBACK
    DefWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (m_fnOldWndProc)
            return ::CallWindowProc(m_fnOldWndProc, hwnd, uMsg, wParam, lParam);
        return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    BOOL Attach(HWND hwnd)
    {
        assert(m_hwnd == NULL);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        m_fnOldWndProc = 
            reinterpret_cast<WNDPROC>(SetWindowLongPtr(hwnd, GWLP_WNDPROC, 
                reinterpret_cast<LONG_PTR>(LineNumBase::WindowProc)));
        m_hwnd = hwnd;
        return TRUE;
    }

    HWND Detach()
    {
        SetWindowLongPtr(m_hwnd, GWLP_USERDATA, 0);
        SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(m_fnOldWndProc));
        m_fnOldWndProc = NULL;
        HWND hwnd = m_hwnd;
        m_hwnd = NULL;
        return hwnd;
    }

    void Redraw(BOOL bErase = FALSE)
    {
        ::InvalidateRect(m_hwnd, NULL, bErase);
    }

protected:
    HWND m_hwnd;
    WNDPROC m_fnOldWndProc;
};

/////////////////////////////////////////////////////////////////////////////////////////
// LineNumStatic --- displays the line number for LineNumEdit

class LineNumStatic : public LineNumBase
{
public:
    LineNumStatic(HWND hwnd = NULL);
    ~LineNumStatic();

    LRESULT CALLBACK
    WindowProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    void SetColors(INT text, INT back, BOOL redraw = TRUE)
    {
        m_rgbText = ::GetSysColor(text);
        m_rgbBack = ::GetSysColor(back);
        if (redraw)
            Redraw();
    }

    void SetLineNumberFormat(LPCTSTR format)
    {
        ::CoTaskMemFree(m_format);
        ::SHStrDup(format, &m_format);
        Redraw();
    }

    HWND GetEdit() const
    {
        return ::GetParent(m_hwnd);
    }

protected:
    COLORREF m_rgbText, m_rgbBack;
    INT m_linedelta;
    LPWSTR m_format;
    HBITMAP m_hbm;
    SIZE m_siz;

    INT GetLineHeight() const
    {
        HDC hDC = ::GetDC(m_hwnd);
        HGDIOBJ hFontOld = ::SelectObject(hDC, GetWindowFont(GetEdit()));
        TEXTMETRIC tm;
        ::GetTextMetrics(hDC, &tm);
        ::SelectObject(hDC, hFontOld);
        ::ReleaseDC(m_hwnd, hDC);
        return tm.tmHeight;
    }

    LPCTSTR GetPropName(INT iLine) const
    {
        static TCHAR s_szProp[64];
        StringCchPrintf(s_szProp, _countof(s_szProp), TEXT("LineNum-%u"), iLine);
        return s_szProp;
    }

    static BOOL CALLBACK
    PropEnumProc(HWND hwnd, LPCTSTR lpszString, HANDLE hData)
    {
        if (HIWORD(lpszString) == 0)
            return TRUE;
        if (::StrCmpNI(lpszString, TEXT("LineNum-"), 8) == 0)
            ::RemoveProp(hwnd, lpszString);
        return TRUE;
    }

    void DeleteProps(HWND hwnd)
    {
        ::EnumProps(hwnd, PropEnumProc);
        Redraw();
    }

    void OnDrawClient(HWND hwnd, HDC hDC);

    friend class LineNumEdit;
};

/////////////////////////////////////////////////////////////////////////////////////////
// LineNumEdit --- textbox with line numbers

class LineNumEdit : public LineNumBase
{
public:
    LineNumEdit(HWND hwnd = NULL)
        : LineNumBase(hwnd)
        , m_num_digits(LINENUMEDIT_DEFAULT_DIGITS)
        , m_cxColumn(0)
    {
    }

    static LPCTSTR SuperWndClassName()
    {
        return TEXT("LineNumEdit");
    }

    void Prepare();

    void RefreshColors()
    {
        if (::IsWindowEnabled(m_hwnd) && !(GetWindowLong(m_hwnd, GWL_STYLE) & ES_READONLY))
            m_hwndStatic.SetColors(COLOR_WINDOWTEXT, COLOR_3DFACE);
        else
            m_hwndStatic.SetColors(COLOR_GRAYTEXT, COLOR_3DFACE);
    }

    void SetLineNumberFormat(LPCTSTR format = NULL)
    {
        if (!format)
            format = TEXT("%d");
        m_hwndStatic.SetLineNumberFormat(format);
    }

    LRESULT CALLBACK
    WindowProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    static LRESULT CALLBACK
    SuperclassWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static WNDPROC SuperclassWindow();

    void SetNumberOfDigits(INT num = LINENUMEDIT_DEFAULT_DIGITS)
    {
        m_cxColumn = 0; // clear cache
        m_num_digits = num;
        Prepare();
    }

protected:
    INT m_num_digits;
    INT m_cxColumn;
    LineNumStatic m_hwndStatic;

    INT GetColumnWidth();
    void UpdateTopAndBottom();
};

#endif  // def LINENUMEDIT_IMPL
