#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "LineNumEdit.hpp"
#include "resource.h"

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    HWND hwndEdit = GetDlgItem(hwnd, edt1);
    if (0)
    {
        ::SendMessage(hwndEdit, LNEM_SETLINEDELTA, 0, 0);
        ::SendMessage(hwndEdit, LNEM_SETLINENUMFORMAT, 0, (LPARAM)TEXT("%08X"));
        ::SendMessage(hwndEdit, LNEM_SETNUMOFDIGITS, 8, 0);
    }
    ::SendMessage(hwndEdit, LNEM_SETLINEMARK, 1, RGB(255, 192, 192));
    ::SendMessage(hwndEdit, LNEM_SETLINEMARK, 3, RGB(0, 0, 0));
    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    }
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    InitCommonControls();
#ifdef LINENUMEDIT_DLL
    LoadLibrary(TEXT("LineNumEdit"));
#else
    LineNumEdit::SuperclassWindow();
#endif
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, DialogProc);
    return 0;
}
