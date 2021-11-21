#pragma once

#include "things_to_do.h"

HWND InitMainWindow();
void InitWindowControls(HWND hWnd);
void InitFromFile(HWND hWnd, LPCTSTR lpszBuffer);
void Resize(HWND hWnd);
void SetTitle(HWND hWnd, LPCWSTR lpszText, BOOL bModifiedMark);