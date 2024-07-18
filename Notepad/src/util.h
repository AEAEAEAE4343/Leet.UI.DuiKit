#pragma once
#include "stdafx.h"

extern WCHAR _szParseError[201];
extern int _dParseError;

void CALLBACK ParserErrorHandler(DirectUI::UCString pszError, DirectUI::UCString pszToken, int dLine, void* showBox);
__int64 DirectUIElementAdd(DirectUI::Element* Parent, DirectUI::Element* Child);