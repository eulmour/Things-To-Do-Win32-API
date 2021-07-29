#include "Modal.h"
#include "resource.h"
#include "WinapiBase/Base.h"

static HWND hEditText = nullptr;
static HWND hColorBox = nullptr;

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

INT_PTR CALLBACK ViewEntry(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static ListView* listView = nullptr;

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
        listView = reinterpret_cast<ListView*>(lParam);

        TCHAR temp_buffer[256] = L"";
        std::wstringstream ss;

        hEditText = GetDlgItem(hDlg, IDC_EDIT_TEXT);
        SetFocus(hEditText);

        if (listView->dwSelected == -1)
        {
            MessageBox(GetParent(hDlg), TEXT("No items in list"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);
            break;
        }

        ListView_GetItemText(*listView, listView->dwSelected, 0 /*col*/, temp_buffer, 256);
        ss << temp_buffer /* << "\r\n" */;

        Edit_SetText(hEditText, ss.str().c_str());

        hColorBox = GetDlgItem(hDlg, IDC_COMBO_COLOR);

        for (int k = 0; k <= 10; ++k)
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
            listView->GetData().at(listView->dwSelected).aTextFields.at(0) = temp_buffer;
            listView->GetData().at(listView->dwSelected).dwColor = colors[listView->dwSelected].color;

            ListView_SetItemText(*listView, listView->dwSelected, 0, temp_buffer);

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