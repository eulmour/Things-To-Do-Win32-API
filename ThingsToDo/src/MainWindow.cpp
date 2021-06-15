#include "MainWindow.h"

INT_PTR CALLBACK AboutBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    //static std::wstring* tr = new std::wstring(proc(L"Cot", L"Cat", L"But"));
    //static int num = proc2(4, 6, 10);

    switch (message)
    {
    case WM_INITDIALOG:

        //static int* inte = new int(proc(5, 4, 6));

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

        INITCOMMONCONTROLSEX icex;
        icex.dwICC = ICC_LISTVIEW_CLASSES;
        InitCommonControlsEx(&icex);
        //MessageBox(m_hWnd, GetErrorMessage(GetLastError()).c_str(), TEXT("Error"), MB_OK | MB_ICONERROR);
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {

        case IDM_ABOUT:
            Dialog(IDD_ABOUTBOX, AboutBox);
            break;

        case IDM_EXIT:
            DestroyWindow(m_hWnd);
            break;

        case ID_BUTTON_1:
            MessageBeep(MB_OK);
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
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);

    }
    return 0;
}
