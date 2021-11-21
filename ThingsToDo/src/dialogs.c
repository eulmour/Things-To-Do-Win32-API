#include "dialogs.h"
#include "list_view.h"
#include "util.h"

#define SET_CLR_ID(source, id) (source |= id << 24)
#define GET_CLR_ID(color) (color >> 24)

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

INT_PTR CALLBACK ViewEntry(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static LONG iItemIndex = -1;
    static int iColorIndex = 0;
    static HWND hEditText = NULL;
    static HWND hColorBox = NULL;
    static ListView* pListView = NULL;

    struct ColorInfo
    {
        LPCTSTR name;
        DWORD color;
    } colors[11] =
    {
        { TEXT("None"),          RGB(255, 255, 255) },
        { TEXT("Red"),           RGB(255, 0, 0) },
        { TEXT("Dark red"),      RGB(127, 0, 0) },
        { TEXT("Gray"),          RGB(190, 190, 190) },
        { TEXT("Blue"),          RGB(180, 180, 255) },
        { TEXT("Dark blue"),     RGB(30, 30, 127) },
        { TEXT("Orange"),        RGB(255, 160, 60) },
        { TEXT("Purple"),        RGB(128, 0, 255) },
        { TEXT("Silver"),        RGB(220, 220, 220) },
        { TEXT("Olive"),         RGB(128, 128, 64) },
        { TEXT("Green"),         RGB(128, 255, 128) }
    };

    switch (message)
    {
    case WM_INITDIALOG:
    {
        pListView = (ListView*)lParam;
        iItemIndex = pListView->dwSelected;

        TCHAR temp_buffer[256] = TEXT("");

        hEditText = GetDlgItem(hDlg, IDC_EDIT_TEXT);
        SetFocus(hEditText);

        if (iItemIndex == -1)
        {
            MessageBox(GetParent(hDlg), TEXT("No items in list"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);
            break;
        }

        ListView_GetItemText(pListView->hWnd, iItemIndex, 0 /*col*/, temp_buffer, 256);

        Edit_SetText(hEditText, temp_buffer);

        hColorBox = GetDlgItem(hDlg, IDC_COMBO_COLOR);

        for (int k = 0; k <= 10; ++k)
        {
            SendMessage(hColorBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)colors[k].name);
        }

        ComboBox_SetCurSel(
            hColorBox,
            GET_CLR_ID(pListView->lwRows[iItemIndex].dwColor));

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
            TCHAR temp_buffer[256] = TEXT("");
            SET_CLR_ID(colors[iColorIndex].color, iColorIndex); /* add an id */

            GetWindowText(hEditText, temp_buffer, 256);

            TCHAR timeBuffer[32];
            GetTime(timeBuffer);

            ListViewReplaceRow(pListView, iItemIndex, (LPCTSTR[2]) {
                temp_buffer,
                timeBuffer
            });

            pListView->lwRows[iItemIndex].dwColor = colors[iColorIndex].color;

            ListView_SetItemText(pListView->hWnd, iItemIndex, 0, temp_buffer);

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }

        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            iColorIndex = ComboBox_GetCurSel(hColorBox);
            return (INT_PTR)TRUE;
        }

        break;
    }
    return (INT_PTR)FALSE;
}
