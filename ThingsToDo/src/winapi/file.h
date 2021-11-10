#pragma once
#include <Windows.h>
#include "../list_view.h"
//void wxFileOpen(HANDLE* hFile, LPCTSTR path);
//void* wxFileRead(HANDLE hFile, size_t size);
//LPTSTR wxFileReadText(HANDLE hFile, size_t size);
//void wxWrite(HANDLE hFile, void* data, size_t size);
//void wxWriteText(HANDLE hFile, LPCTSTR text);
//void wxResetPosition(HANDLE hFile);
//void wxFileClose(HANDLE hFile);

void	FileOpen(HANDLE* hFile, LPCTSTR path);
HRESULT WriteDataToFile(ListView* lw);
HRESULT ReadDataFromFile(ListView* lw);
void	WriteTextToFile(LPCTSTR lpszText, DWORD dwTextSize, HANDLE hFile);
void	WriteRow(ListViewRow* lwRow, DWORD dwRowSize, HANDLE hFile);

//void	WriteRow(DWORD dwColor, BOOL bChecked, std::vector<std::wstring> aTextFields, HANDLE hFile);
//void	ReadTextFromFile(std::vector<std::wstring>& aTextFields, OVERLAPPED& ol, HANDLE hFile);
//void	ReadRowFromFile(std::vector<ListViewRow>& aRows, DWORD dwSize, OVERLAPPED& ol, HANDLE hFile);