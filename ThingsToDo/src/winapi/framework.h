// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <ShlObj.h>
#include <Shlwapi.h>

#define ZeroStruct(s) RtlSecureZeroMemory(&s, sizeof(s))
#define SendListViewMessage(hwnd, id, msg)\
    {\
        NMHDR hNf = { .code = msg };\
        SendMessage(hwnd, WM_NOTIFY, id, (LPARAM)&hNf);\
    }
#define ArrayEnd(arr) (&arr[0] + (sizeof(arr) / sizeof(arr[0])))
#define Distance(begin, end) ((end - begin) * sizeof(*begin))
#define RandFloat(max) (((float)rand() / (float)(RAND_MAX)) * max)
