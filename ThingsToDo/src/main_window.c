#include "main_window.h"
#include "main_window_msg.h"
#include "Resource.h"
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

void InitWindowControls(HWND hWnd)
{
    assert(
        ListViewInit(&lw, IDC_LIST_ENTRIES, hWnd) &&
        "Error during ListViewInit: Unable to initialize window."
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

void InitFromFile()
{
    // TODO: Add default file loader
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
