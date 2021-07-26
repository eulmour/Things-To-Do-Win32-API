#include "MainWindow.h"

namespace
{
    HWND hMainWindow    = nullptr;
    HWND hListView      = nullptr;
    HWND hEditText      = nullptr;
    HWND hColorBox      = nullptr;

    DWORD   listViewStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS;
    LPCWSTR pszColumnNames[2] = { L"Note", L"Modified" };
    DWORD   pdwColumnsWidth[2] = { 700, 120 };
    ListView listView = { { 0, 0, 516, 559 }, 2, pszColumnNames, pdwColumnsWidth, ID_MAIN_LIST, listViewStyle };
} // end of anonimous namespace

void MainWindow::InitWindowControls()
{
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
    ListView_SetColumnWidth(listView, 0, static_cast<DWORD>(rect.right) - nOffset);

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

INT_PTR CALLBACK ViewEntry(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam /*Item index*/)
{
    static int iItemIndex;

    static struct ColorInfo
    {
        LPCWSTR name;
        DWORD color;
    } colors[11] =
    {
        { TEXT("None"),          0xFFFFFF },
        { TEXT("Red"),           0xFF0000 },
        { TEXT("Dark red"),      0x770000 },
        { TEXT("Gray"),          0x888888 },
        { TEXT("Blue"),          0x1133FF },
        { TEXT("Dark blue"),     0x000088 },
        { TEXT("Orange"),        0xFFFFFF },
        { TEXT("Purple"),        0xFFFFFF },
        { TEXT("Silver"),        0xFFFFFF },
        { TEXT("Olive"),         0xFFFFFF },
        { TEXT("Green"),         0xFFFFFF }
    };

    switch (message)
    {
    case WM_INITDIALOG:
    {
        iItemIndex = static_cast<int>(lParam);

        TCHAR temp_buffer[256] = L"";
        std::wstringstream ss;

        hEditText = GetDlgItem(hDlg, IDC_EDIT_TEXT);
        SetFocus(hEditText);

        if (iItemIndex == -1)
        {
            MessageBox(GetParent(hDlg), TEXT("No items in list"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);
            break;
        }

        ListView_GetItemText(hListView, iItemIndex, 0 /*col*/, temp_buffer, 256);
        ss << temp_buffer /* << "\r\n" */;

        Edit_SetText(hEditText, ss.str().c_str());

        hColorBox = GetDlgItem(hDlg, IDC_COMBO_COLOR);

        for (int  k = 0; k <= 10; ++k)
        {
            SendMessage(hColorBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)colors[k].name);
        }

        ComboBox_SetCurSel(hColorBox, 0);

        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:

        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }

        if (LOWORD(wParam) == IDOK)
        {
            TCHAR temp_buffer[256] = L"";

            GetWindowTextW(hEditText, temp_buffer, 256);
            listView.GetData().at(iItemIndex).aTextFields.at(0) = temp_buffer;
            listView.GetData().at(iItemIndex).dwColor = colors[iItemIndex].color;

            ListView_SetItemText(hListView, iItemIndex, 0, temp_buffer);

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }

        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            int ItemIndex = ComboBox_GetCurSel(hColorBox);
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
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        InitCommonControlsEx(&icex);
        InitWindowControls();
        InitFileHierarchy();
        hMainWindow = m_hWnd;
        hListView = listView;

        /*MessageBox(m_hWnd, GetErrorMessage(GetLastError()).c_str(), TEXT("Error"), MB_OK | MB_ICONERROR);*/
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

        case ID_EDIT_CUT: /* TODO cut item */

        {
            DWORD dwSelected = ListView_GetNextItem(listView, -1, LVNI_SELECTED);

            if (dwSelected == -1)
                break;

            ListView_DeleteItem(listView, dwSelected);
        }

            break;

        case ID_EDIT_COPY: /* TODO copy item */



            break;

        case ID_EDIT_PASTE: /* TODO paste item */



            break;

        case ID_EDIT_SELECTALL:

            SendListViewMessage(m_hWnd, ID_MAIN_LIST, LM_SELECTALL);

            break;
        case ID_EDIT_INSERTNEW:
        {
            listView.AddRow<0xFFFFFF, FALSE>(L"", GetTime());
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

    case NF_MSG:
        if (wParam == ID_MAIN_LIST)
            //LoadLibrary(TEXT("Riched20.dll"));
            Dialog(IDD_VIEW_ENTRY, ViewEntry, lParam);
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
