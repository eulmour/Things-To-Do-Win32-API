#pragma once
#include "framework.h"

BOOL			ShowFileDialog(LPCTSTR szFilter, LPCTSTR lpszBuffer, HWND hWnd, BOOL bSave);
LPCTSTR			GetLastErrorMessage();
LPCTSTR			GetTime(LPCTSTR pBuffer);
