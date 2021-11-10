#include "file.h"
#include "util.h"

static HANDLE hFile;
static OVERLAPPED ol = { 0 };
static float* buffer = NULL;
static LPTSTR textBuffer = NULL;

void FileOpen(HANDLE* hFile, LPCTSTR path)
{
    if (FileExists(path))
    {
        *hFile = HandleFile(path, FALSE);
    }
}

HRESULT WriteDataToFile(ListView* lw)
{
    DWORD nBytesWritten = 0UL;

    // Rows
    DWORD count         = lw->cRows;
    ListViewRow* begin  = lw->lwRows[0];
    ListViewRow* end    = lw->lwRows[lw->cRows];

    // 1. Write row count
    WriteFile(
        hFile,
        &count,
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

HRESULT ReadDataFromFile(ListView* lw)
{
    OVERLAPPED ol = { 0 };
    ol.Offset = 0;

    ListView_DeleteAllItems(lw->hWnd);
    ListViewDestroy(lw);

    assert(
        ListViewInit(&lw, IDC_LIST_ENTRIES, GetParent(lw->hWnd)) &&
        "Error during ListViewInit: Unable to initialize window."
    );

    //static ListViewCol rgColumns[2] =
    //{
    //    {.dwWidth = 700, .lpszName = TEXT("Task") },
    //    {.dwWidth = 120, .lpszName = TEXT("Modified") }
    //};

    //ListViewSetColumns(lw, 2, rgColumns);

    DWORD dwColumnCount = 0;
    DWORD dwRowCount = 0;

    // 1. Read column count
    // 2. Read column name size
    // 3. Read column name
    // 4. Repeat utill the zero count
    // 5. Read row count
    // 6. Read color
    // 7. Read checked
    // 8. Read row fields until the zero column count
    // 9. Repeat until zero row count

    // 1. Get the column count
    ReadFile(
        hFile,
        &dwColumnCount,
        sizeof(DWORD),
        NULL,
        &ol
    );

    // 2. Update the offset to next value in the file
    ol.Offset += sizeof(DWORD);

    // 3. Exit if file is empty
    if (!dwColumnCount)
        return S_FALSE;

    // 4. Read column names

    for (size_t i = 0; i < dwColumnCount; i++)
    {
        //ReadTextFromFile(lw->lwCols, &ol, hFile);
    }

    // 5. Get the row count
    ReadFile(
        hFile,
        &dwRowCount,
        sizeof(DWORD),
        NULL,
        &ol
    );

    // 6. Update the offset to next value in the file
    ol.Offset += sizeof(DWORD);

    // 7. Exit if no rows
    if (!dwRowCount)
        return S_OK;

    // 8. Read row data
    for (size_t i = 0; i < dwRowCount; i++)
    {
        //ReadRowFromFile(taskList.GetData(), dwColumnCount, ol, hFile);
    }

    CloseHandle(hFile);
    return S_OK;
}

void WriteTextToFile(LPCTSTR lpszText, DWORD dwTextSize, HANDLE hFile)
{
    DWORD nBytesWritten = 0UL;

    // 1. Write text size first
    //DWORD dwTextSize = static_cast<DWORD>(sText.size()) * sizeof(WCHAR) + sizeof(WCHAR);

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

void WriteRow(ListViewRow* lwRow, DWORD dwRowSize, HANDLE hFile)
{
    DWORD nBytesWritten = 0UL;
    DWORD dwNameSize = 0UL;

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

    //ListViewText* begin = &lwRow->rgText[0];
    //ListViewText* end = &lwRow->rgText[dwRowSize];

    //while (begin != end)
    //{
        //WriteTextToFile(*begin, hFile);
        //++begin;
    //}
}

//void ReadTextFromFile(ListViewCol* rgTextFields, OVERLAPPED* ol, HANDLE hFile)
//{
//    DWORD dwLengthRetrieved = 0;
//    //std::unique_ptr<WCHAR[]> pCharBuffer;
//
//    // 1. Read the header and get the size of next byte sequence
//    ReadFile(
//        hFile,
//        &dwLengthRetrieved,
//        sizeof(DWORD),
//        NULL,
//        ol
//    );
//
//    // 2. Update the offset to current character in the file
//    ol->Offset += sizeof(DWORD);
//
//    // 3. Alocate new free memory to store a data we'll get in the next func
//    pCharBuffer.reset(new WCHAR[dwLengthRetrieved]);
//
//    LPTSTR lpCharBuffer = (LPTSTR)malloc(dwLengthRetrieved * sizeof(TCHAR));
//
//    // 4. Read char sequence lLengthRetrieved bytes long and store in the buffer
//    ReadFile(
//        hFile,
//        pCharBuffer.get(),
//        dwLengthRetrieved,
//        NULL,
//        ol
//    );
//    
//    ol->Offset += dwLengthRetrieved;
//
//    // 5. Update column names
//    aTextFields.push_back(std::wstring(pCharBuffer.get()));
//}

//void ReadRowFromFile(std::vector<ListViewRow>& aRows, DWORD dwSize, OVERLAPPED& ol, HANDLE hFile)
//{
//    DWORD           dwLengthRetrieved = 0;
//    ListViewRow     newRow{ 0, 0, std::vector<std::wstring>() };
//
//    // 1. Get color
//    ReadFile(
//        hFile,
//        &newRow.dwColor,
//        sizeof(DWORD),
//        nullptr,
//        &ol
//    );
//
//    // 2. Update the offset to current character in the file
//    ol.Offset += sizeof(DWORD);
//
//    // 3. Get check state
//    ReadFile(
//        hFile,
//        &newRow.bChecked,
//        sizeof(BOOL),
//        nullptr,
//        &ol
//    );
//
//    // 4. Update the offset to current character in the file
//    ol.Offset += sizeof(BOOL);
//
//    for (size_t i = 0; i < dwSize; i++)
//    {
//        ReadTextFromFile(newRow.aTextFields, ol, hFile);
//    }
//
//    // 5. Insert row
//    aRows.push_back(newRow);
//}