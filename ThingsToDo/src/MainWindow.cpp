#include "MainWindow.h"

void MainWindow::InitWindowControls()
{
    //mainWindow->Append(&button1);
    //mainWindow->Append(&label1);

    AppendList(&listView, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
    SetFocus(listView);
}

void MainWindow::InitFileHierarchy()
{
    WCHAR szCurrentFilePath[MAX_PATH * sizeof(WCHAR)];
        
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, szCurrentFilePath)))
    {
        PathAppend(szCurrentFilePath, TEXT("\\ThingsToDo"));

        // if no app folder found create that one
        if (!DirectoryExists(szCurrentFilePath))
        {
            CreateDirectory(szCurrentFilePath, NULL);
        }

        PathAppend(szCurrentFilePath, TEXT("\\data.txt"));
        sCurrentFilePath = szCurrentFilePath;

        // if file isn't found create it
        if (!FileExists(szCurrentFilePath))
        {
            SendMessage(m_hWnd, WM_COMMAND, ID_FILE_SAVE, NULL);
            return;
        }

        m_hFile = HandleFile(sCurrentFilePath.c_str(), FALSE);

        if (_ReadComplexFromFile() == S_FALSE) MessageBox(m_hWnd, L"Error. File not found", L"Error", MB_OK | MB_ICONERROR);
        listView.UpdateList();

    }
}

void MainWindow::ResizeList()
{
    RECT rect;
    DWORD nOffset = 0;

    for (size_t i = 0; i < listView.GetHeader().size(); ++i)
    {
        if (i == 0)
            continue;
        nOffset += pdwColumnsWidth[i];
    }

    GetClientRect(listView, &rect);
    ListView_SetColumnWidth(listView, 0, rect.right - nOffset);

    HWND parent = GetParent(listView);
    GetClientRect(parent, &rect);
    MoveWindow(listView, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
}

INT_PTR CALLBACK AboutBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:

        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

LRESULT MainWindow::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        INITCOMMONCONTROLSEX icex;
        icex.dwICC = ICC_LISTVIEW_CLASSES;
        InitCommonControlsEx(&icex);
        InitWindowControls();
        InitFileHierarchy();

        //MessageBox(m_hWnd, GetErrorMessage(GetLastError()).c_str(), TEXT("Error"), MB_OK | MB_ICONERROR);
        break;
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        // Parse the menu selections:
        switch (wmId)
        {

        case ID_FILE_NEW:
            listView.Clear();
            break;

        case ID_FILE_OPEN:

            sCurrentFilePath = ShowFileDialog(L"Text files\0*.txt\0All files\0*.*\0", m_hWnd, FALSE);
            SetTitle(sCurrentFilePath.c_str());

            m_hFile = HandleFile(sCurrentFilePath.c_str(), FALSE);

            listView.Clear();
            if (_ReadComplexFromFile() == S_FALSE) MessageBox(m_hWnd, L"Error. File not found", L"Error", MB_OK | MB_ICONERROR);
            listView.UpdateList();

            break;

        case ID_FILE_SAVE:

            m_hFile = HandleFile(sCurrentFilePath.c_str(), TRUE);

            if (_WriteComplexToFile() == S_FALSE) MessageBox(m_hWnd, L"Error while saving file", L"Error", MB_OK | MB_ICONERROR);

            break;

        case ID_FILE_SAVEAS:

            sCurrentFilePath = ShowFileDialog(L"Text files\0*.txt\0All files\0*.*\0", m_hWnd, TRUE);
            SetTitle(sCurrentFilePath.c_str());
            PostMessage(m_hWnd, WM_COMMAND, ID_FILE_SAVE, lParam);

            break;

        case ID_EDIT_INSERTNEW:
        {
            SYSTEMTIME st = { 0 };
            GetLocalTime(&st);
            wchar_t cTimeStr[32];
            swprintf_s(cTimeStr, L"%02d/%02d/%d %02d:%02d:%02d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
            listView.AddRow<0xFFFFFF, FALSE>(L"", (const wchar_t*)cTimeStr);
            listView.UpdateList();
            break;
        }

        case ID_EDIT_DELETE:
            //TODO: delete selection
            listView.DeleteItem();
            break;

        case IDM_ABOUT:
            Dialog(IDD_ABOUTBOX, AboutBox);
            break;

        case IDM_EXIT:
            DestroyWindow(m_hWnd);
            break;

        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }
    }
    break;

    case WM_NOTIFY:

        for (const auto& listView : m_aListViewList)
        {
            if (listView->GetId() == LOWORD(wParam))
                listView->OnNotify(lParam);
        }

        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hWnd, &ps);

        EndPaint(m_hWnd, &ps);
    }
    break;

    case WM_SIZE:
        ResizeList();

        if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
            ResizeList();

        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);

    }
    return 0;
}

void MainWindow::SetTitle(LPCWSTR lpText)
{
    WCHAR temp[256];
    swprintf_s(temp, L"Things To Do - %s", lpText);
    SetWindowText(m_hWnd, temp);
}

HRESULT MainWindow::_WriteComplexToFile()
{
    DWORD nBytesWritten = 0UL;

    // Columns
    DWORD headerSize    = static_cast<DWORD>(listView.GetHeader().size());
    auto headerBegin    = listView.GetHeader().begin();
    auto headerEnd      = listView.GetHeader().end();

    // 1. Write column count
    WriteFile(
        m_hFile,
        &headerSize,
        sizeof(DWORD),
        &nBytesWritten,
        nullptr
    );

    // 2. Write column names
    while (headerBegin != headerEnd)
    {
        WriteTextToFile(*headerBegin, m_hFile);
        ++headerBegin;
    }

    // Rows
    DWORD count = static_cast<DWORD>(listView.GetData().size());
    auto begin = listView.GetData().begin();
    auto end = listView.GetData().end();

    // 1. Write row count
    WriteFile(
        m_hFile,
        &count,
        sizeof(DWORD),
        &nBytesWritten,
        nullptr
    );

    // 2. Write row data
    while (begin != end)
    {
        WriteRow(begin->dwColor, begin->bChecked, begin->aTextFields, m_hFile);
        ++begin;
    }

    CloseHandle(m_hFile);
    return S_OK;
}

HRESULT MainWindow::_ReadComplexFromFile()
{
    OVERLAPPED ol = { 0 };
    ol.Offset = 0;

    listView.GetHeader().clear();
    listView.GetData().clear();

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
        m_hFile,
        &dwColumnCount,
        sizeof(DWORD),
        nullptr,
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
        ReadTextFromFile(listView.GetHeader(), ol, m_hFile);
    }

    // 5. Get the row count
    ReadFile(
        m_hFile,
        &dwRowCount,
        sizeof(DWORD),
        nullptr,
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
        ReadRowFromFile(listView.GetData(), dwColumnCount, ol, m_hFile);
    }

    CloseHandle(m_hFile);
    return S_OK;
}
