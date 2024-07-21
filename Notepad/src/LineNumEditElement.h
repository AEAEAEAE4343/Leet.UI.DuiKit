#pragma once
#include "stdafx.h"
#include "NotepadListener.h"

class LineNumEditElement : public DirectUI::Element
{
public:
    // ClassInfo accessors (static and virtual instance-based)
    static DirectUI::IClassInfo* Class;
    virtual DirectUI::IClassInfo* GetClassInfo() { return Class; }
    static HRESULT Register();
    static inline DirectUI::UCString DoGetClassName() { return (DirectUI::UCString)L"LineNumEdit"; }

    virtual UINT MessageCallback(GMSG* pMsg) override;
    virtual void OnHosted(DirectUI::Element*) override;
    virtual void OnInput(DirectUI::InputEvent*) override;
    virtual void OnPropertyChanged(DirectUI::PropertyInfo*, int index, Value* pOld, Value* pNew) override;

    static HRESULT CreateInstance(DirectUI::Element* rootElement, unsigned long* debugVariable, DirectUI::Element** newElement);
    
    void SyncVisible();
    void SyncRect(UINT nChangeFlags, bool bForceSync = false);
protected:
    virtual HWND CreateHWND(HWND hwndParent);

private:
    void SyncParent();
    void SyncFont();
    void SyncText();

    static LRESULT sinkSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    static LRESULT ctrlSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

    WNDPROC _oldSinkProc = 0;
    WNDPROC _oldCtrlProc = 0;
    HWND _hWndSink = 0;
    HWND _hWndCtrl = 0;
};

