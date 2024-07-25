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
 * util.h/util.cpp: Several utility functions
 *
 */

#include "util.h"

WCHAR _szParseError[201];
int _dParseError;

void CALLBACK ParserErrorHandler(DirectUI::UCString pszError, DirectUI::UCString pszToken, int dLine, void* showBox)
{
    if (dLine != -1)
        swprintf(_szParseError, L"%s '%s' at line %d", pszError, pszToken, dLine);
    else
        swprintf(_szParseError, L"%s '%s'", pszError, pszToken);

    if (*reinterpret_cast<bool*>(showBox))
        MessageBoxW(NULL, _szParseError, L"DUIXML Parser Error", MB_OK);
}

__int64 DirectUIElementAdd(DirectUI::Element* Parent, DirectUI::Element* Child)
{
    // Thanks wiktorwiktor12 for pointing me to this!
    // https://github.com/wiktorwiktor12/ConsoleLogonHook/blob/dde28b32488d5bdf27e75bdc2d0152c44788b31b/ConsoleLogonUI/util/util.h#L84
    return (*(__int64(__fastcall**)(DirectUI::Element*, DirectUI::Element**, __int64))(*(uintptr_t*)Parent + 128i64))(
        Parent,
        (DirectUI::Element**)&Child,
        1i64);
}