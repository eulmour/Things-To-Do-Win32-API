#include "file.h"
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

void FileOpen(HANDLE* hFile, LPCTSTR path)
{
    if (FileExists(path))
    {
        *hFile = HandleFile(path, FALSE);
    }
}

HRESULT WriteDataToFile(ListView* lw, HANDLE hFile)
{
    DWORD nBytesWritten = 0UL;

    // Rows
    ListViewRow* begin  = &lw->lwRows[0];
    ListViewRow* end    = &lw->lwRows[lw->cRows];

    // 1. Write row count
    WriteFile(
        hFile,
        (LPCVOID) & lw->cRows,
        sizeof(DWORD),
        &nBytesWritten,
        NULL
    );

    // 2. Write row data
    while (begin != end)
    {
        WriteRow(begin, lw->cCols, hFile);
        ++begin;
    }

    CloseHandle(hFile);
    return S_OK;
}

void WriteRow(ListViewRow* lwRow, DWORD dwRowSize, HANDLE hFile)
{
    DWORD nBytesWritten = 0UL;
    //DWORD dwNameSize = 0UL;

    //--- 1. Write color.
    WriteFile(
        hFile,
        &lwRow->dwColor,
        sizeof(DWORD),
        &nBytesWritten,
        NULL
    );

    // 2. Write check status
    WriteFile(
        hFile,
        &lwRow->bChecked,
        sizeof(BOOL),
        &nBytesWritten,
        NULL
    );

    ListViewText* begin = &lwRow->rgText[0];
    ListViewText* end = &lwRow->rgText[dwRowSize];

    while (begin != end)
    {
        WriteTextToFile(begin->lpszText, (begin->dwSize + 1) * sizeof(TCHAR), hFile);
        ++begin;
    }
}

void WriteTextToFile(LPCTSTR lpszText, DWORD dwTextSize, HANDLE hFile)
{
    DWORD nBytesWritten = 0UL;

    // 1. Write text size first
    WriteFile(
        hFile,
        &dwTextSize,
        sizeof(DWORD),
        &nBytesWritten,
        NULL
    );

    // 2. Then write text
    WriteFile(
        hFile,
        (LPCVOID)lpszText,
        dwTextSize,
        &nBytesWritten,
        NULL
    );
}

HRESULT ReadDataFromFile(ListView* lw, HANDLE hFile)
{
    OVERLAPPED ol = { 0 };
    ol.Offset = 0;

    // 1. Get the row count
    ReadFile(
        hFile,
        &lw->cPending,
        sizeof(DWORD),
        NULL,
        &ol
    );

    // 2. Update the offset to next value in the file
    ol.Offset += sizeof(DWORD);

    // 3. Exit if no rows
    if (!lw->cPending)
        return S_FALSE;

    // 4. Read row data
    for (size_t i = 0; i < lw->cPending; i++)
    {
        ReadRowFromFile(&lw->lwRows[i], lw->cCols, &ol, hFile);
    }

    CloseHandle(hFile);
    return S_OK;
}

void ReadRowFromFile(ListViewRow* lwRow, DWORD dwSize, OVERLAPPED* ol, HANDLE hFile)
{
    DWORD dwLengthRetrieved = 0;

    // 1. Get color
    ReadFile(
        hFile,
        (LPVOID) & lwRow->dwColor,
        sizeof(DWORD),
        NULL,
        ol
    );

    // 2. Update the offset to current character in the file
    ol->Offset += sizeof(DWORD);

    // 3. Get check state
    ReadFile(
        hFile,
        (LPVOID) & lwRow->bChecked,
        sizeof(BOOL),
        NULL,
        ol
    );

    // 4. Update the offset to current character in the file
    ol->Offset += sizeof(BOOL);

    // 5. Allocate text fields space
    lwRow->rgText = (ListViewText*)malloc(sizeof(ListViewText) * dwSize);

    for (size_t i = 0; i < dwSize; i++)
    {
        ReadTextFromFile(&lwRow->rgText[i], ol, hFile);
    }
}

void ReadTextFromFile(ListViewText* lwText, OVERLAPPED* ol, HANDLE hFile)
{
    DWORD dwLengthRetrieved = 0;

    // 1. Read the header and get the size of next byte sequence
    ReadFile(
        hFile,
        &dwLengthRetrieved,
        sizeof(DWORD),
        NULL,
        ol
    );

    // 2. Update the offset to current character in the file
    ol->Offset += sizeof(DWORD);

    // 3. Alocate new free memory to store a data we'll get in the next func
    lwText->dwSize = dwLengthRetrieved;
    lwText->lpszText = (LPCTSTR)malloc(dwLengthRetrieved * sizeof(TCHAR));

    // 4. Read char sequence lLengthRetrieved bytes long and store in the buffer
    ReadFile(
        hFile,
        (LPVOID)lwText->lpszText,
        dwLengthRetrieved,
        NULL,
        ol
    );

    ol->Offset += dwLengthRetrieved;
}