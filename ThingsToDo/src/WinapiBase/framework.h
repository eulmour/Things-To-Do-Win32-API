// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// C RunTime Header Files
#include <tchar.h>
// std headers
#include <memory>
#include <string>
#include <string_view>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <type_traits>

// Windows Header Files
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <ShlObj.h>
#include <Shlwapi.h>

//#include <shobjidl.h>
//#include <atlbase.h>

#include "Utils.h"

#define ZeroStruct(s) RtlSecureZeroMemory(&s, sizeof(s))
#define NF_MSG (WM_USER + 0x1000)
#define MKLONG(loword, hiword) (loword | (hiword << 16 | 0)) 

#define SendListViewMessage(hwnd, id, msg)\
    {\
        NMHDR hNf = { .code = msg };\
        SendMessage(hwnd, WM_NOTIFY, id, (LPARAM)&hNf);\
    }