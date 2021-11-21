#include "main_window.h"
#include "main_window_msg.h"
#include "resource.h"
#include "list_view.h"

ListView lw;

HWND InitMainWindow()
{
    return CreateDialogParam(
        GetModuleHandle(NULL),
        MAKEINTRESOURCE(IDD_DIALOG_MAIN),
        NULL,
        (DLGPROC)WndProc,
        NULL);
}

void WINAPI InitWindowControls(HWND hWnd)
{
    Try(
        ListViewInit(&lw, IDC_LIST_ENTRIES, hWnd) == FALSE,
        TEXT("Error during ListViewInit: Unable to initialize window.")
    );

    ListViewSetStyle(&lw,
        LVS_EX_CHECKBOXES |
        LVS_EX_FULLROWSELECT |
        LVS_EX_HEADERDRAGDROP);

    SetFocus(lw.hWnd);

    static ListViewCol rgColumns[2] =
    {
        {.dwWidth = 700, .lpszName = TEXT("Task") },
        {.dwWidth = 120, .lpszName = TEXT("Modified") }
    };

    ListViewSetColumns(&lw, 2, rgColumns);
}

void InitFromFile(HWND hWnd, LPCTSTR lpszBuffer)
{
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, lpszBuffer)))
    {
        PathAppend(lpszBuffer, TEXT("\\ThingsToDo"));

        // if no app folder found create that one
        if (!DirectoryExists(lpszBuffer))
        {
            CreateDirectory(lpszBuffer, NULL);
        }

        PathAppend(lpszBuffer, TEXT("\\data.txt"));

        // if file isn't found create it
        if (!FileExists(lpszBuffer))
        {
            SendMessage(hWnd, WM_COMMAND, ID_FILE_SAVE, NULL);
        }
        else
        {
            SendMessage(hWnd, WM_COMMAND, ID_FILE_OPEN, lpszBuffer);
        }
    }
}

void Resize(HWND hWnd)
{
    RECT rect;
    DWORD nOffset = 0;

    for (size_t i = 0; i < lw.cCols; ++i)
    {
        if (i == 0)
            continue;
        nOffset += lw.lwCols[i].dwWidth;
    }

    GetClientRect(lw.hWnd, &rect);
    ListView_SetColumnWidth(lw.hWnd, 0, (DWORD)rect.right - nOffset);

    GetClientRect(hWnd, &rect);
    MoveWindow(lw.hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
}

void SetTitle(HWND hWnd, LPCWSTR lpszText, BOOL bModifiedMark)
{
    TCHAR temp[256];

    if (bModifiedMark)
        _stprintf_s(temp, 256, TEXT("Things To Do - %s*"), lpszText);
    else
        _stprintf_s(temp, 256, TEXT("Things To Do - %s"), lpszText);

    SetWindowText(hWnd, temp);
}
