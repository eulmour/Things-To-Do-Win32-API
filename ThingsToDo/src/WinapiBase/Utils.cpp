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
            GENERIC_WRITE,
            FILE_SHARE_READ,
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
