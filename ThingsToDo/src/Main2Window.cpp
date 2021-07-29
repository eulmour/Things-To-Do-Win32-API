#include "Main2Window.h"
#include "Modal.h"

INT_PTR Main2Window::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        //SetMenu(hDlg, LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_THINGSTODO)));
        INITCOMMONCONTROLSEX icex;
        icex.dwICC = ICC_LISTVIEW_CLASSES;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        InitCommonControlsEx(&icex);
        InitWindowControls();
        InitFileHierarchy();

        //When list view would be initialized you must set this:
        SetWindowLongPtr(listView, GWLP_USERDATA, (LONG_PTR)&listView);

        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        switch (wmId)
        {

        case IDOK:
        {
            PostQuitMessage(0);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }

        case IDCANCEL:
        {
            PostQuitMessage(0);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }

        case ID_FILE_NEW:
            listView.Clear();
            return (INT_PTR)TRUE;

        case ID_FILE_OPEN:
        {
            sCurrentFilePath = ShowFileDialog(L"Text files\0*.txt\0All files\0*.*\0", hDlg, FALSE);
            SetTitle(sCurrentFilePath.c_str());

            hFile = HandleFile(sCurrentFilePath.c_str(), FALSE);

            listView.Clear();
            if (ReadDataFromFile() == S_FALSE) MessageBox(hDlg, L"Error. File not found", L"Error", MB_OK | MB_ICONERROR);
            listView.UpdateList();

            return (INT_PTR)TRUE;
        }

        case ID_FILE_SAVE:
        {
            hFile = HandleFile(sCurrentFilePath.c_str(), TRUE);

            if (WriteDataToFile() == S_FALSE) MessageBox(hDlg, L"Error while saving file", L"Error", MB_OK | MB_ICONERROR);

            return (INT_PTR)TRUE;
        }

        case ID_EDIT_CUT: /* TODO cut item */
        {
            DWORD dwSelected = ListView_GetNextItem(listView, -1, LVNI_SELECTED);

            if (dwSelected == -1)
                return (INT_PTR)TRUE;

            ListView_DeleteItem(listView, dwSelected);
            return (INT_PTR)TRUE;
        }

        case ID_EDIT_COPY: /* TODO copy item */



            return (INT_PTR)TRUE;

        case ID_EDIT_PASTE: /* TODO paste item */



            return (INT_PTR)TRUE;

        case ID_EDIT_SELECTALL:
        {
            SendListViewMessage(hDlg, IDC_LIST_ENTRIES, LM_SELECTALL);
            return (INT_PTR)TRUE;
        }

        case ID_EDIT_INSERTNEW:
        {
            listView.AddRow<0xFFFFFF, FALSE>(L"", GetTime());
            listView.UpdateList();
            return (INT_PTR)TRUE;
        }

        case ID_EDIT_DELETE:
            //TODO: delete selection
            listView.DeleteItem();
            return (INT_PTR)TRUE;

        case IDM_ABOUT:

            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), hDlg, AboutBox);
            return (INT_PTR)TRUE;

        case IDM_EXIT:

            DestroyWindow(hDlg);
            return (INT_PTR)TRUE;

        }

        break;
    } /* end of WM_COMMAND */

    case WM_NOTIFY:
    {
        if (listView.GetId() == LOWORD(wParam))
            listView.OnNotify(lParam);

        return (INT_PTR)NULL;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hDlg, &ps);

        EndPaint(hDlg, &ps);

        return (INT_PTR)TRUE;
    }

    case WM_SIZE:
        ResizeList();

        if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
            ResizeList();

        return (INT_PTR)NULL;

    case WM_CLOSE:

        PostQuitMessage(0);
        EndDialog(hDlg, NULL);

        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

void Main2Window::InitWindowControls()
{
    listView.Init(hDlg);
    listView.SetStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
}

void Main2Window::InitFileHierarchy()
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
            SendMessage(hDlg, WM_COMMAND, ID_FILE_SAVE, NULL);
            return;
        }

        hFile = HandleFile(sCurrentFilePath.c_str(), FALSE);

        if (ReadDataFromFile() == S_FALSE) MessageBox(hDlg, L"Error. File not found", L"Error", MB_OK | MB_ICONERROR);

        listView.UpdateList();
    }
}

void Main2Window::ResizeList()
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

void Main2Window::SetTitle(LPCWSTR lpText)
{
    WCHAR temp[256];
    swprintf_s(temp, L"Things To Do - %s", lpText);
    SetWindowText(hDlg, temp);
}

HRESULT Main2Window::WriteDataToFile()
{
    DWORD nBytesWritten = 0UL;

    // Columns
    DWORD headerSize = static_cast<DWORD>(listView.GetHeader().size());
    auto headerBegin = listView.GetHeader().begin();
    auto headerEnd = listView.GetHeader().end();

    // 1. Write column count
    WriteFile(
        hFile,
        &headerSize,
        sizeof(DWORD),
        &nBytesWritten,
        nullptr
    );

    // 2. Write column names
    while (headerBegin != headerEnd)
    {
        WriteTextToFile(*headerBegin, hFile);
        ++headerBegin;
    }

    // Rows
    DWORD count = static_cast<DWORD>(listView.GetData().size());
    auto begin = listView.GetData().begin();
    auto end = listView.GetData().end();

    // 1. Write row count
    WriteFile(
        hFile,
        &count,
        sizeof(DWORD),
        &nBytesWritten,
        nullptr
    );

    // 2. Write row data
    while (begin != end)
    {
        WriteRow(begin->dwColor, begin->bChecked, begin->aTextFields, hFile);
        ++begin;
    }

    CloseHandle(hFile);
    return S_OK;
}

HRESULT Main2Window::ReadDataFromFile()
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
        hFile,
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
        ReadTextFromFile(listView.GetHeader(), ol, hFile);
    }

    // 5. Get the row count
    ReadFile(
        hFile,
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
        ReadRowFromFile(listView.GetData(), dwColumnCount, ol, hFile);
    }

    CloseHandle(hFile);
    return S_OK;
}
