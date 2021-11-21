#include "util.h"

BOOL ShowFileDialog(LPCTSTR szFilter, LPCTSTR lpszBuffer, HWND hWnd, BOOL bSave)
{
    static OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = lpszBuffer;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = szFilter;
    ofn.nFilterIndex = 1;

    if (bSave == TRUE)
        return GetSaveFileName(&ofn);
    else
        return GetOpenFileName(&ofn);
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
