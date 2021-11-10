#pragma once

#include "things_to_do.h"

HWND InitMainWindow();
void InitWindowControls(HWND hWnd);
void InitFromFile();
void Resize(HWND hWnd);
void SetTitle(LPCWSTR lpszText);