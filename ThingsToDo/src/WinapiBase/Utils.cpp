#include "Utils.h"

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

std::wstring ShowFileDialog(LPCWSTR szFilter, HWND hWnd, BOOL bSave)
{
    TCHAR cFilePath[MAX_PATH];
    OPENFILENAME ofn;
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

    return std::wstring{ ofn.lpstrFile };
}

void WriteTextToFile(std::wstring& sText, HANDLE hFile)
{
    DWORD nBytesWritten = 0UL;

    // 1. Write text size first
    DWORD dwTextSize = static_cast<DWORD>(sText.size()) * sizeof(WCHAR) + sizeof(WCHAR);

    WriteFile(
        hFile,
        &dwTextSize,
        sizeof(DWORD),
        &nBytesWritten,
        nullptr
    );

    // 2. Then write text
    WriteFile(
        hFile,
        sText.c_str(),
        dwTextSize,
        &nBytesWritten,
        nullptr
    );
}

void WriteRow(DWORD dwColor, BOOL bChecked, std::vector<std::wstring> aTextFields, HANDLE hFile)
{
    DWORD nBytesWritten = 0UL;
    DWORD dwNameSize = 0UL;

    //--- 1. Write color.
    WriteFile(
        hFile,
        &dwColor,
        sizeof(DWORD),
        &nBytesWritten,
        nullptr
    );

    // 2. Write check status
    WriteFile(
        hFile,
        &bChecked,
        sizeof(BOOL),
        &nBytesWritten,
        nullptr
    );

    auto begin = aTextFields.begin();
    auto end = aTextFields.end();

    while (begin != end)
    {
        WriteTextToFile(*begin, hFile);
        ++begin;
    }
}

void ReadTextFromFile(std::vector<std::wstring>& aTextFields, OVERLAPPED& ol, HANDLE hFile)
{
    DWORD dwLengthRetrieved = 0;
    std::unique_ptr<WCHAR[]> pCharBuffer;

    // 1. Read the header and get the size of next byte sequence
    ReadFile(
        hFile,
        &dwLengthRetrieved,
        sizeof(DWORD),
        nullptr,
        &ol
    );

    // 2. Update the offset to current character in the file
    ol.Offset += sizeof(DWORD);

    // 3. Alocate new free memory to store a data we'll get in the next func
    pCharBuffer.reset(new WCHAR[dwLengthRetrieved]);

    // 4. Read char sequence lLengthRetrieved bytes long and store in the buffer
    ReadFile(
        hFile,
        pCharBuffer.get(),
        dwLengthRetrieved,
        nullptr,
        &ol
    );
    ol.Offset += dwLengthRetrieved;

    // 5. Update column names
    aTextFields.push_back(std::wstring(pCharBuffer.get()));
}

void ReadRowFromFile(std::vector<ListViewRow>& aRows, DWORD dwSize, OVERLAPPED& ol, HANDLE hFile)
{
    DWORD           dwLengthRetrieved = 0;
    ListViewRow     newRow{ 0, 0, std::vector<std::wstring>() };

    // 1. Get color
    ReadFile(
        hFile,
        &newRow.dwColor,
        sizeof(DWORD),
        nullptr,
        &ol
    );

    // 2. Update the offset to current character in the file
    ol.Offset += sizeof(DWORD);

    // 3. Get check state
    ReadFile(
        hFile,
        &newRow.bChecked,
        sizeof(BOOL),
        nullptr,
        &ol
    );

    // 4. Update the offset to current character in the file
    ol.Offset += sizeof(BOOL);

    for (size_t i = 0; i < dwSize; i++)
    {
        ReadTextFromFile(newRow.aTextFields, ol, hFile);
    }

    // 5. Insert row
    aRows.push_back(newRow);
}

HANDLE HandleFile(LPCWSTR lpPath, BOOL bSave)
{
    HANDLE hFile = nullptr;

    if (bSave == false)
    {
        hFile = CreateFileW(
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
        hFile = CreateFileW(
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

const wchar_t* GetTime()
{
    static wchar_t cTimeStr[32];

    SYSTEMTIME st = { 0 };
    GetLocalTime(&st);
    swprintf_s(cTimeStr, L"%02d/%02d/%d %02d:%02d:%02d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);

    return &cTimeStr[0];
}

std::basic_string<TCHAR> GetLastErrorMessage()
{
    LPTSTR psz = NULL;
    const DWORD cchMsg = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
        | FORMAT_MESSAGE_IGNORE_INSERTS
        | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL, // (not used with FORMAT_MESSAGE_FROM_SYSTEM)
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPTSTR>(&psz),
        0,
        NULL);
    if (cchMsg > 0)
    {
        // Assign buffer to smart pointer with custom deleter so that memory gets released
        // in case String's c'tor throws an exception.
        auto deleter = [](void* p) { ::HeapFree(::GetProcessHeap(), 0, p); };
        std::unique_ptr<TCHAR, decltype(deleter)> ptrBuffer(psz, deleter);
        return std::basic_string<TCHAR>(ptrBuffer.get(), cchMsg);
    }
    else
    {
        throw std::runtime_error("Failed to retrieve error message string.");
    }
}

LRESULT ProcessCustomDraw(LPARAM lParam)
{
    LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;

    switch (lplvcd->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT: //Before the paint cycle begins
        //request notifications for individual listview items
        return CDRF_NOTIFYITEMDRAW;

    case CDDS_ITEMPREPAINT: //Before an item is drawn
        if (((int)lplvcd->nmcd.dwItemSpec % 2) == 0)
        {
            //customize item appearance
            lplvcd->clrText = RGB(255, 0, 0);
            lplvcd->clrTextBk = RGB(200, 200, 200);
            return CDRF_NEWFONT;
        }
        else {
            lplvcd->clrText = RGB(0, 0, 255);
            lplvcd->clrTextBk = RGB(255, 255, 255);

            return CDRF_NEWFONT;
        }
        break;

        //Before a subitem is drawn
    case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
        if (0 /*TODO*/ == (int)lplvcd->nmcd.dwItemSpec)
        {
            if (0 == lplvcd->iSubItem)
            {
                //customize subitem appearance for column 0
                lplvcd->clrText = RGB(255, 0, 0);
                lplvcd->clrTextBk = RGB(255, 255, 255);

                //To set a custom font:
                //SelectObject(lplvcd->nmcd.hdc, 
                //    <your custom HFONT>);

                return CDRF_NEWFONT;
            }
            else if (1 == lplvcd->iSubItem)
            {
                //customize subitem appearance for columns 1..n
                //Note: setting for column i 
                //carries over to columnn i+1 unless
                //      it is explicitly reset
                lplvcd->clrTextBk = RGB(255, 0, 0);
                lplvcd->clrTextBk = RGB(255, 255, 255);

                return CDRF_NEWFONT;
            }
        }
    }
    return CDRF_DODEFAULT;
}