#pragma once
#include "framework.h"

BOOL            FileExists(LPCTSTR szPath);
BOOL            DirectoryExists(LPCTSTR szPath);
HANDLE          HandleFile(LPCTSTR lpPath, BOOL bSave);
LPCTSTR			ShowFileDialog(LPCTSTR szFilter, HWND hWnd, BOOL bSave);
LPCTSTR			GetLastErrorMessage();
LPCTSTR			GetTime(LPCTSTR pBuffer);
