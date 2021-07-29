#include "MainWindow.h"
#include "Modal.h"

namespace
{
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
            SendMessage(hWnd, WM_COMMAND, ID_FILE_SAVE, NULL);
            return;
        }

        m_hFile = HandleFile(sCurrentFilePath.c_str(), FALSE);

        if (ReadDataFromFile() == S_FALSE) MessageBox(hWnd, L"Error. File not found", L"Error", MB_OK | MB_ICONERROR);
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



LRESULT MainWindow::OnMessage(HWND hHandle, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

        break;
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        // Parse the menu selections:
        switch (wmId)
        {

        case ID_FILE_NEW:
        {
            listView.Clear();
            break;
        }

        case ID_FILE_OPEN:
        {
            sCurrentFilePath = ShowFileDialog(L"Text files\0*.txt\0All files\0*.*\0", hWnd, FALSE);
            SetTitle(sCurrentFilePath.c_str());

            m_hFile = HandleFile(sCurrentFilePath.c_str(), FALSE);

            listView.Clear();
            if (ReadDataFromFile() == S_FALSE) MessageBox(hWnd, L"Error. File not found", L"Error", MB_OK | MB_ICONERROR);
            listView.UpdateList();

            break;
        }

        case ID_FILE_SAVE:
        {
            m_hFile = HandleFile(sCurrentFilePath.c_str(), TRUE);
            if (WriteDataToFile() == S_FALSE) MessageBox(hWnd, L"Error while saving file", L"Error", MB_OK | MB_ICONERROR);
            break;
        }

        case ID_FILE_SAVEAS:
        {
            sCurrentFilePath = ShowFileDialog(L"Text files\0*.txt\0All files\0*.*\0", hWnd, TRUE);
            SetTitle(sCurrentFilePath.c_str());
            PostMessage(hWnd, WM_COMMAND, ID_FILE_SAVE, lParam);

            break;
        }

        case ID_EDIT_CUT: /* TODO cut item */
        {
            DWORD dwSelected = ListView_GetNextItem(listView, -1, LVNI_SELECTED);

            if (dwSelected == -1)
                break;

            ListView_DeleteItem(listView, dwSelected);
            break;
        }

        case ID_EDIT_COPY: /* TODO copy item */



            break;

        case ID_EDIT_PASTE: /* TODO paste item */



            break;

        case ID_EDIT_SELECTALL:
        {
            SendListViewMessage(hWnd, ID_MAIN_LIST, LM_SELECTALL);
            break;
        }

        case ID_EDIT_INSERTNEW:
        {
            listView.AddRow<0xFFFFFF, FALSE>(L"", GetTime());
            listView.UpdateList();
            break;
        }

        case ID_EDIT_DELETE:
        {
            //TODO: delete selection
            listView.DeleteItem();
            break;
        }

        case IDM_ABOUT:
        {
            Dialog(IDD_ABOUTBOX, AboutBox);
            break;
        }

        case IDM_EXIT:
        {
            DestroyWindow(hWnd);
            break;
        }

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
    break;

    case WM_NOTIFY:
    {
        for (const auto& listView : aListViewList)
        {
            if (listView->GetId() == LOWORD(wParam))
            {
                LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

                if (pnm->hdr.hwndFrom == *listView && pnm->hdr.code == NM_CUSTOMDRAW)
                {
                    SetWindowLongPtr(hHandle, 0, (LONG)ProcessCustomDraw(lParam, listView->dwSelected));
                    break;
                }

                listView->OnNotify(lParam);
            }
        }

        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_SIZE:
    {
        ResizeList();

        if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
            ResizeList();

        break;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }

    case NF_MSG:
    {
        if (wParam == ID_MAIN_LIST)
            Dialog(IDD_VIEW_ENTRY, ViewEntry, (LPARAM)&listView);

        break;
    }

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);

    }
    return 0;
}

void MainWindow::SetTitle(LPCWSTR lpText)
{
    WCHAR temp[256];
    swprintf_s(temp, L"Things To Do - %s", lpText);
    SetWindowText(hWnd, temp);
}

HRESULT MainWindow::WriteDataToFile()
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

HRESULT MainWindow::ReadDataFromFile()
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
