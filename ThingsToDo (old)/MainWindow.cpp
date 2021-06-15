#include "MainWindow.h"

#include <Commdlg.h>
//#include <commctrl.h>
#include <Shlobj.h>
#include <shobjidl.h>
#include <atlbase.h>

#include "Shlwapi.h"
#include <fstream>

//--- List id
#define IDC_LIST (WM_USER + 0x9001)
#define IDC_LIST_2 (WM_USER + 0x9002)

//--- Custom list messages
#define LM_DO_FINDFIRST (WM_USER + 0x0300)
#define LM_DO_EDITCANCEL (WM_USER + 0x0301)

//--- List properties
#define LV_COLUMN_COUNT 3
#define LV_ROW_COUNT 10

//#define FINDMSGSTRING TEXT("commdlg_FindReplace")

MainWindow::MainWindow()
    : m_nScreenWidth(GetSystemMetrics(SM_CXSCREEN)),
    m_nScreenHeight(GetSystemMetrics(SM_CYSCREEN)),
    m_wCurrentFilePath(nullptr),
    m_ListView({ {L"Note", 700 }, {L"Created", 120} })
{
}

/*{L"Created", L"Done", L"Note" }*/
//{ wwt::Column(L"Created", 80), wwt::Column(L"Done", 80), wwt::Column(L"Note", 600) }

INT_PTR CALLBACK About(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (uMsg)
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

INT_PTR CALLBACK FindFirst(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (uMsg)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:

        if (LOWORD(wParam) == IDOK)
        {
            const int nWidth = GetWindowTextLengthW(GetDlgItem(hDlg, IDC_EDIT_FIND)) + 1;

            if (nWidth > 0)
            {
                //wchar_t* sEditCtrlText = (wchar_t*)malloc(nWidth * sizeof(wchar_t));
                wchar_t* sEditCtrlText = new wchar_t [nWidth * sizeof(wchar_t)];

                if (sEditCtrlText)
                {
                    GetWindowTextW(GetDlgItem(hDlg, IDC_EDIT_FIND), sEditCtrlText, nWidth);

                    //Use received text from edit control here
                    NMHDR nmh;
                    nmh.code = LM_DO_FINDFIRST;
                    nmh.idFrom = GetDlgCtrlID(hDlg);
                    nmh.hwndFrom = hDlg;
                    SendMessage(GetParent(hDlg), WM_NOTIFY, IDC_LIST, (LPARAM)&nmh);
                }
                delete[] sEditCtrlText;
            }

            //TODO: Implement search logic
        }
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg)
    {
    case WM_CREATE:

        // Structure for control initialization.
        INITCOMMONCONTROLSEX icex;
        icex.dwICC = ICC_LISTVIEW_CLASSES;
        InitCommonControlsEx(&icex);

        //--- List view creation
        m_ListView.CreateEx(
            L"",
            //WS_TABSTOP |
            WS_CHILD |
            //WS_BORDER |
            WS_VISIBLE |
            //LVS_AUTOARRANGE |
            LVS_REPORT |
            LVS_EDITLABELS /*|
            LVS_SORTASCENDING |
            LVS_OWNERDATA*/,
            NULL,
            GetClientAreaSize().left,
            GetClientAreaSize().top,
            GetClientAreaSize().right,
            GetClientAreaSize().bottom,
            m_hWnd,
            (HMENU)IDC_LIST);

        m_ListView.SetStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
        //m_ListView.SetStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
        //m_ListView.InitListViewImageLists(IDI_SMALL);
        m_ListView.Initialize();

        // Open the file stored in Appdata directory immediately
        m_wCurrentFilePath.reset(new WCHAR[MAX_PATH * sizeof(WCHAR)]);
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, m_wCurrentFilePath.get())))
        {
            PathAppend(m_wCurrentFilePath.get(), TEXT("\\TODO32List32\\text.txt"));

            m_hFile = CreateFileW(
                m_wCurrentFilePath.get(),
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_WRITE | FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, NULL
            );

            if (m_hFile == INVALID_HANDLE_VALUE)
            {
                MessageBeep(MB_ICONERROR);
                MessageBoxW(m_hWnd, L"ERROR_WHILE_OPENING_FILE", L"Warning", MB_OK);
                break;
            }

            if (_ReadComplexFromFile() == S_FALSE) MessageBox(m_hWnd, L"Error. File not found", L"Error", MB_OK | MB_ICONERROR);
            m_ListView.UpdateList();
        }

        // Initialize notification handle
        m_hNotification.idFrom = NULL;
        m_hNotification.hwndFrom = m_hWnd;
        break;
    case WM_SIZE:
        //static RECT rect;
        //GetClientRect(m_hWnd, &rect);
        //ResizeListView();
        m_ListView.ResizeView(0);
        if(wParam == SIZE_MAXIMIZED)
            m_ListView.ResizeView(0);
        if (wParam == SIZE_RESTORED)
            m_ListView.ResizeView(0);
        //--- TODO: Redraw
        break;;
    case WM_NOTIFY:
        if (LOWORD(wParam) == IDC_LIST)
            m_ListView.OnNotify(lParam);
        //if (LOWORD(wParam) == IDC_LIST_2)
        //    another_view_proc(m_hWnd, lParam);
        break;
    case WM_INITMENUPOPUP:
        //lListView.UpdateMenu(GetMenu(m_hWnd));
        break;
    case WM_CLOSE:
        DestroyWindow(m_hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case ID_FILE_NEW:
            m_hNotification.code = LVM_DELETEALLITEMS;
            SendMessage(m_hWnd, WM_NOTIFY, IDC_LIST, (LPARAM)&m_hNotification);
            break;
        case IDM_FILE_OPEN:

            ShowFileDialog();
            SetTitle(m_wCurrentFilePath.get());
            m_hFile = CreateFileW(m_wCurrentFilePath.get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            if (m_hFile == INVALID_HANDLE_VALUE)
            {
                MessageBeep(MB_ICONERROR);
                MessageBoxW(m_hWnd, L"ERROR_WHILE_OPENING_FILE", L"Warning", MB_OK);
                break;
            }

            m_ListView.Clear();
            if (_ReadComplexFromFile() == S_FALSE) MessageBox(m_hWnd, L"Error. File not found", L"Error", MB_OK | MB_ICONERROR);
            m_ListView.UpdateList();

            break;

        case ID_FILE_SAVE:

            m_hFile = CreateFileW(m_wCurrentFilePath.get(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

            if (m_hFile == INVALID_HANDLE_VALUE)
            {
                MessageBeep(MB_ICONERROR);
                MessageBoxW(m_hWnd, L"ERROR_WHILE_SAVING_FILE", L"Warning", MB_OK);
                PostMessage(m_hWnd, WM_COMMAND, ID_FILE_SAVEAS, lParam);
                break;
            }

            m_ListView.UpdateCheckBoxState();
            if (_WriteComplexToFile() == S_FALSE) MessageBox(m_hWnd, L"Error while saving file", L"Error", MB_OK | MB_ICONERROR);

            break;

        case IDM_FILE_SAVEAS:
            //TODO: Implement file creation dialog
            ShowFileDialog(TRUE);
            SetTitle(m_wCurrentFilePath.get());
            MessageBoxW(NULL, m_wCurrentFilePath.get(), L"File Path", MB_OK);

            m_hFile = CreateFileW(m_wCurrentFilePath.get(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

            if (m_hFile == INVALID_HANDLE_VALUE)
            {
                MessageBeep(MB_ICONERROR);
                MessageBoxW(m_hWnd, L"ERROR_WHILE_SAVING_FILE", L"Warning", MB_OK);
            }

            m_ListView.UpdateCheckBoxState();
            if (_WriteComplexToFile() == S_FALSE) MessageBox(m_hWnd, L"Error while saving file", L"Error", MB_OK | MB_ICONERROR);

            break;
        case ID_EDIT_UNDO:
            break;
        case ID_EDIT_REDO:
            break;
        case ID_EDIT_SELECTALL:
            m_ListView.HighlightItems();
            break;
        case ID_EDIT_INSERTNEW:
            GetLocalTime(&st);
            TCHAR cTimeStr[32];
            swprintf_s(cTimeStr, L"%02d/%02d/%d %02d:%02d:%02d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
            m_ListView.InsertItem(0, L"");
            m_ListView.InsertItem(1, cTimeStr);
            m_ListView.UpdateList();
            break;
        case ID_EDIT_DELETESELECTED:
            //TODO: delete selection
            m_ListView.PopItem();
            break;
        case IDM_EDIT_FIND:
            //RegisterWindowMessageW(WM_USER + 0x0001);
            //static auto hDlgFind = CreateDialogW(m_hInst, TEXT("Find"), 
            static TCHAR whatFind[80];
            whatFind[0] = L'H';

            RegisterWindowMessage(TEXT("FINDMSGSTRING"));

            FINDREPLACE fr;
            ZeroMemory(&fr, sizeof(FINDREPLACE));
            fr.lStructSize = sizeof(FINDREPLACE);
            //fr.hInstance = m_hInst;
            fr.hwndOwner = m_hWnd;
            fr.Flags = FR_DIALOGTERM | FR_FINDNEXT | FR_REPLACE | FR_REPLACEALL;
            fr.lpstrFindWhat = whatFind;
            fr.wFindWhatLen = sizeof(whatFind);
            fr.lpstrReplaceWith = whatFind;
            fr.wReplaceWithLen = sizeof(whatFind);
            fr.hInstance = m_hInst;
            //fr.lpTemplateName
            ReplaceText(&fr);  
            static auto errorVal = CommDlgExtendedError();

            //DialogBoxParam(m_hInst, MAKEINTRESOURCE(IDD_FIND), m_hWnd, FindFirst, 0L);
            break;
        case IDM_ABOUT:
            DialogBoxParamW(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), m_hWnd, About, 0L);
            break;
        case IDM_EXIT:
            DestroyWindow(m_hWnd);
            break;
        case IDK_CANCEL:
            //--- TODO: Handle esc key
            m_hNotification.code = LM_DO_EDITCANCEL;
            //m_hNotification.code = LVN_ENDLABELEDIT;
            SendMessage(m_hWnd, WM_NOTIFY, IDC_LIST, (LPARAM)&m_hNotification);
            break;
        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }
    }
    break;
    default:
        break;
    }

    //return TRUE;
    return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

void MainWindow::ShowFileDialog(BOOL bSave)
{
    TCHAR cFilePath[MAX_PATH];
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFile = cFilePath;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Text files\0*.txt\0All files\0*.*\0";
    ofn.nFilterIndex = 1;

    if (bSave == TRUE)
        GetSaveFileName(&ofn);
    else
        GetOpenFileName(&ofn);

    m_wCurrentFilePath.reset(new WCHAR[MAX_PATH * sizeof(WCHAR)]);
    m_wCurrentFilePath[0] = L'\0';
    _tcsncpy_s(m_wCurrentFilePath.get(), ofn.nMaxFile, ofn.lpstrFile, ofn.nMaxFile);
}

RECT MainWindow::GetClientAreaSize() const
{
    RECT rect;
    GetClientRect(m_hWnd, &rect);
    return rect;
}

HRESULT MainWindow::_WriteComplexToFile()
{
    DWORD dwHeader = 0UL;
    DWORD nBytesWritten = 0UL;

    auto begin = m_ListView.RetrieveData().begin();
    auto end = m_ListView.RetrieveData().end();

    m_ListView.UpdateCheckBoxState();


    while (begin != end)
    {
        //--- Retrieve column
        auto pElemPtr = &*begin;

        //--- Retrieve column properties
        std::wstring* caption = &pElemPtr->caption;
        int* size = &pElemPtr->size;
        std::vector<std::pair<std::wstring, int>>* items = &pElemPtr->items;

        //--- 1. Write caption size in the header.
        //--- Note that I used "+ sizeof(WCHAR)" to store null character in each string end
        dwHeader = static_cast<DWORD>(caption->size()) * sizeof(WCHAR) + sizeof(WCHAR);
        WriteFile(
            m_hFile,
            &dwHeader,
            sizeof(DWORD),
            &nBytesWritten,
            nullptr
        );

        //--- 1. Write caption
        WriteFile(
            m_hFile,
            caption->c_str(),
            static_cast<DWORD>(caption->size()) * sizeof(WCHAR) + sizeof(WCHAR),
            &nBytesWritten,
            nullptr
        );

        //--- 2. Write size
        WriteFile(
            m_hFile,
            size,
            sizeof(int),
            &nBytesWritten,
            nullptr
        );

        //--- 3. Write items container
        auto pItemBegin = items->begin();
        auto pItemEnd = items->end();
        while (pItemBegin != pItemEnd)
        {
            auto pItemPtr = &*pItemBegin;

            dwHeader = static_cast<DWORD>(pItemPtr->first.size()) * sizeof(WCHAR) + sizeof(WCHAR);
            WriteFile(
                m_hFile,
                &dwHeader,
                sizeof(DWORD),
                &nBytesWritten,
                nullptr
            );

            //--- Text
            WriteFile(
                m_hFile,
                pItemPtr->first.c_str(),
                static_cast<DWORD>(pItemPtr->first.size()) * sizeof(WCHAR) + sizeof(WCHAR),
                &nBytesWritten,
                nullptr
            );

            //--- Checkbox status
            WriteFile(
                m_hFile,
                &pItemPtr->second,
                sizeof(int),
                &nBytesWritten,
                nullptr
            );
            //--- Next row
            ++pItemBegin;
        }

        //--- End of column
        dwHeader = -1;
        WriteFile(
            m_hFile,
            &dwHeader,
            sizeof(DWORD),
            &nBytesWritten,
            nullptr
        );

        //--- Next column
        ++begin;
    }


    m_dwLastError = ::GetLastError();
    CloseHandle(m_hFile);
    return S_OK;
}

HRESULT MainWindow::_ReadComplexFromFile()
{
    DWORD lLengthRetrieved = 0;
    std::unique_ptr<WCHAR[]> pCharBuffer;
    int* pIntBuffer = new int(0);

    m_ListView.RetrieveData()[0].items.clear();
    m_ListView.RetrieveData()[1].items.clear();
    auto begin = m_ListView.RetrieveData().begin();
    auto end = m_ListView.RetrieveData().end();

    OVERLAPPED ol = { 0 };
    ol.Offset = 0;

    while (begin != end)
    {
        //--- Get the column
        auto pColumn = &*begin;

        //--- Read the header and get the size of next byte sequence
        ReadFile(
            m_hFile,
            &lLengthRetrieved,
            sizeof(DWORD),
            nullptr,
            &ol
        );
        //--- Update the offset to current character in the file
        ol.Offset += sizeof(DWORD);

        //--- Leave if file is empty
        if (!lLengthRetrieved)
            return S_FALSE;

        //--- Alocate new free memory to store a data we'll get in the next func
        pCharBuffer.reset(new WCHAR[lLengthRetrieved]);

        //--- Read char sequence lLengthRetrieved bytes long and store in the buffer
        ReadFile(
            m_hFile,
            pCharBuffer.get(),
            lLengthRetrieved,
            nullptr,
            &ol
        );
        ol.Offset += lLengthRetrieved;

        //--- Update column names
        pColumn->caption = std::wstring(pCharBuffer.get());

        //--- Read int lLengthRetrieved bytes long and store in the buffer
        ReadFile(
            m_hFile,
            pIntBuffer,
            sizeof(int),
            nullptr,
            &ol
        );
        
        pColumn->size = *pIntBuffer;
        ol.Offset += sizeof(int);

        //--- Retreive item collection
        auto items = &pColumn->items;
        while (true) // until the end of the file
        {
            ReadFile(
                m_hFile,
                &lLengthRetrieved,
                sizeof(DWORD),
                nullptr,
                &ol
            );
            ol.Offset += sizeof(DWORD);

            if (lLengthRetrieved == -1)
                break;

            pCharBuffer.reset(new WCHAR[lLengthRetrieved]);


            ReadFile(
                m_hFile,
                pCharBuffer.get(),
                lLengthRetrieved,
                nullptr,
                &ol
            );
            ol.Offset += lLengthRetrieved;

            ReadFile(
                m_hFile,
                pIntBuffer,
                sizeof(int),
                nullptr,
                &ol
            );
            ol.Offset += sizeof(int);

            items->push_back(std::make_pair(pCharBuffer.get(), *pIntBuffer));
        }

        ++begin;
    }

    if (pIntBuffer)
        delete pIntBuffer;

    m_dwLastError = ::GetLastError();
    CloseHandle(m_hFile);
    return S_OK;
}

void MainWindow::SetTitle(LPCWSTR lpText)
{
    WCHAR temp[256];
    swprintf_s(temp, L"Things To Do - %s", lpText);
    SetWindowText(m_hWnd, temp);
}
