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

#pragma once
#include "stdafx.h"

extern WCHAR _szParseError[201];
extern int _dParseError;

void CALLBACK ParserErrorHandler(DirectUI::UCString pszError, DirectUI::UCString pszToken, int dLine, void* showBox);
__int64 DirectUIElementAdd(DirectUI::Element* Parent, DirectUI::Element* Child);