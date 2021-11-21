#pragma once
#include <Windows.h>
#include "../list_view.h"


BOOL    FileExists			(LPCTSTR szPath);
BOOL    DirectoryExists		(LPCTSTR szPath);
HANDLE  HandleFile			(LPCTSTR lpPath, BOOL bSave);
void	FileOpen			(HANDLE* hFile, LPCTSTR path);
BOOL	FileExists			(LPCTSTR szPath);
BOOL	DirectoryExists		(LPCTSTR szPath);
HRESULT WriteDataToFile		(ListView* lw, HANDLE hFile);
void	WriteRow			(ListViewRow* lwRow, DWORD dwRowSize, HANDLE hFile);
void	WriteTextToFile		(LPCTSTR lpszText, DWORD dwTextSize, HANDLE hFile);
HRESULT ReadDataFromFile	(ListView* lw, HANDLE hFile);
void	ReadRowFromFile		(ListViewRow* lwRow, DWORD dwSize, OVERLAPPED* ol, HANDLE hFile);
void	ReadTextFromFile	(ListViewText* lwText, OVERLAPPED* ol, HANDLE hFile);