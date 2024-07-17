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
 * stdafx.h: Include standard libraries 
 *
 */

#ifndef DUI_APP_STDAFX_H_INCLUDED
#define DUI_APP_STDAFX_H_INCLUDED

#pragma once

// Use only needed Win32 functions
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

// Win32 Headers
#include <windows.h>
#include <windowsx.h>
#include <math.h>

// Include COM
#include <objbase.h>

// Include GDI+
#ifdef GADGET_ENABLE_GDIPLUS
#pragma warning(push, 3)
#include <GdiPlus.h>            // GDI+
#pragma warning(pop)
#endif // GADGET_ENABLE_GDIPLUS

// Include C Standard Library
#include <stdlib.h>
#include <malloc.h>
#include <wchar.h>
#include <process.h>

// Included for theme support
#include <uxtheme.h>

// Shell libs
#include <ocidl.h>
#include <shellapi.h>
#include <shlwapi.h>

// Include DirectUI 7 headers
#define GADGET_ENABLE_TRANSITIONS
//#include <duser.h>
#include "../DirectUI/include/DirectUI.h"

#endif // DUI_APP_STDAFX_H_INCLUDED
