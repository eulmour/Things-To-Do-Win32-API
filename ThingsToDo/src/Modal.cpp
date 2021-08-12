#include "TaskList.h"
#include "Modal.h"

#define SET_CLR_ID(source, id) (source |= id << 24)
#define GET_CLR_ID(color) (color >> 24)

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

INT_PTR CALLBACK ViewEntry(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam /* listview hwnd*/)
{
    static LONG iItemIndex = -1;
    static int iColorIndex = 0;
    static HWND hEditText = nullptr;
    static HWND hColorBox = nullptr;
    static TaskList* pNoteList = nullptr;

    static struct ColorInfo
    {
        LPCWSTR name;
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
        pNoteList = reinterpret_cast<TaskList*>(lParam);
        iItemIndex = pNoteList->dwSelected;

        TCHAR temp_buffer[256] = L"";
        std::wstringstream ss;

        hEditText = GetDlgItem(hDlg, IDC_EDIT_TEXT);
        SetFocus(hEditText);

        if (iItemIndex == -1)
        {
            MessageBox(GetParent(hDlg), TEXT("No items in list"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);
            break;
        }

        ListView_GetItemText(*pNoteList, iItemIndex, 0 /*col*/, temp_buffer, 256);
        ss << temp_buffer /* << "\r\n" */;

        Edit_SetText(hEditText, ss.str().c_str());

        hColorBox = GetDlgItem(hDlg, IDC_COMBO_COLOR);

        for (int k = 0; k <= 10; ++k)
        {
            SendMessage(hColorBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)colors[k].name);
        }

        ComboBox_SetCurSel(
            hColorBox,
            GET_CLR_ID(pNoteList->GetData().at(iItemIndex).dwColor));

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
            SET_CLR_ID(colors[iColorIndex].color, iColorIndex); /* add an id */

            GetWindowTextW(hEditText, temp_buffer, 256);
            pNoteList->GetData().at(iItemIndex).aTextFields.at(0) = temp_buffer;
            pNoteList->GetData().at(iItemIndex).aTextFields.at(1) = GetTime();
            pNoteList->GetData().at(iItemIndex).dwColor = colors[iColorIndex].color;

            ListView_SetItemText(*pNoteList, iItemIndex, 0, temp_buffer);

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