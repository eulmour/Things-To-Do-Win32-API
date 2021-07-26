#pragma once
#include <framework.h>

struct Position
{
    DWORD x;
    DWORD y;
    DWORD width;
    DWORD height;
};

struct ListViewRow
{
    BOOL bChecked;
    DWORD dwColor;
    std::vector<std::wstring> aTextFields;
};

template <typename T, typename ...Ts>
using are_same = std::conjunction<std::is_same<T, Ts>...>;

BOOL            FileExists(LPCTSTR szPath);
BOOL            DirectoryExists(LPCTSTR szPath);
std::wstring    ShowFileDialog  (LPCWSTR szFilter, HWND hWnd, BOOL bSave);
void            WriteTextToFile (std::wstring& sText, HANDLE hFile);
void            WriteRow        (DWORD dwColor, BOOL bChecked, std::vector<std::wstring> aTextFields, HANDLE hFile);
void            ReadTextFromFile(std::vector<std::wstring>& aTextFields, OVERLAPPED& ol, HANDLE hFile);
void            ReadRowFromFile(std::vector<ListViewRow>& aRows, DWORD dwSize, OVERLAPPED& ol, HANDLE hFile);
HANDLE          HandleFile(LPCWSTR lpPath, BOOL bSave);
const wchar_t*  GetTime();
std::basic_string<TCHAR> GetLastErrorMessage();