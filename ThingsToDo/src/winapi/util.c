#include "util.h"

BOOL FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL DirectoryExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

HANDLE HandleFile(LPCTSTR lpPath, BOOL bSave)
{
    HANDLE hFile = NULL;

    if (bSave == FALSE)
    {
        hFile = CreateFile(
            lpPath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    }
    else
    {
        hFile = CreateFile(
            lpPath,
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    }

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MessageBeep(MB_ICONERROR);
        MessageBoxW(NULL, L"ERROR_WHILE_PROCESSING_FILE", L"Warning", MB_OK);
    }

    return hFile;
}

LPCTSTR ShowFileDialog(LPCTSTR szFilter, HWND hWnd, BOOL bSave)
{
    static TCHAR cFilePath[MAX_PATH];
    static OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = cFilePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = szFilter;
    ofn.nFilterIndex = 1;

    if (bSave == TRUE)
        GetSaveFileName(&ofn);
    else
        GetOpenFileName(&ofn);

    return ofn.lpstrFile;
}

LPCTSTR GetLastErrorMessage()
{
    static TCHAR psz[80];
    psz[0] = '\0';

    const DWORD cchMsg = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        psz,
        0,
        NULL);

    if (cchMsg > 0)
    {
        return (LPCTSTR)&psz[0];
    }
    else
    {
        // Failed to retrieve error message string
    }
}

LPCTSTR GetTime(LPCTSTR pBuffer)
{
    //static TCHAR cTimeStr[32];

    SYSTEMTIME st = { 0 };
    GetLocalTime(&st);

    _stprintf_s(pBuffer, 32, TEXT("%02d/%02d/%d %02d:%02d:%02d"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);

    return pBuffer;
}
