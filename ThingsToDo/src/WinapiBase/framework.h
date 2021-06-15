// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// C RunTime Header Files
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
#include <tchar.h>
// std headers
#include <memory>
//#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <type_traits>

// Windows Header Files
#include <windows.h>
#include <commctrl.h>

#include "Utils.h"

//#include <Commdlg.h>
//#include <Shlobj.h>
//#include <shobjidl.h>
//#include <atlbase.h>

//#include "Shlwapi.h"

#define ZeroStruct(s) RtlSecureZeroMemory(&s, sizeof(s))
