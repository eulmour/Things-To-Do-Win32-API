#include "MainWindow.h"
#include <vector>
#define ID_BUTTON_1 (WM_USER + 0x1001)

template <typename ... Param>
auto proc(const Param& ... param)
{
    std::wstringstream ss;
    //using List = int[];
    using List = int[];
    (void)List {0, ((void)(ss << param), 0) ... };

    return ss.str();
}

template <typename ... Param>
auto proc2(const Param& ... param)
{
    static int temp = 0;
    temp += ((param + 100) + ...);
    return temp;
}

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
    static TCHAR chTable[] = { L'♥', L'☼', L'◊', L'♦', L'♀', L'♂', L'♫', L'🐳', L'Ѽ', L'҈', L'\0' };
    static std::vector<std::vector<Button*>> vecvecBtn;
    static RECT rect;
    //AdjustWindowRect(&rect, NULL, TRUE);
    static float nWidth =     0.0f;
    static float nHeight =    0.0f;
    static int nSquareWidth =    0;
    static int nSquareHeight =   0;

    //std::wstring chTab = std::wstring(chTable);

    switch (uMsg)
    {
    case WM_CREATE:
        //static Button* button = new Button(TEXT("Press the button"), m_hWnd, 0, 140, 100, 20);

        GetClientRect(m_hWnd, &rect);
        //AdjustWindowRect(&rect, NULL, TRUE);

        nSquareWidth =  floorf(((rect.right - rect.left) / 10));
        nSquareHeight =  floorf(((rect.bottom - rect.top) / 10));

        for (int i = 0; i < floor((rect.bottom - rect.top)); i += nSquareHeight)
        {
            std::vector<Button*> vecBtn;
            for (int j = 0; j < floorf((rect.right - rect.left)); j += nSquareWidth)
            {
                vecBtn.push_back(new Button(std::wstring(chTable).c_str(), m_hWnd, j, i, nSquareWidth, nSquareHeight, ID_BUTTON_1));
            }
            vecvecBtn.push_back(vecBtn);
        }

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
            //DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), m_hWnd, About);
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
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hWnd, &ps);

        EndPaint(m_hWnd, &ps);
    }
    break;
    case WM_SIZE:
        GetClientRect(m_hWnd, &rect);
        //AdjustWindowRect(&rect, NULL, TRUE);
        nWidth = rect.right - rect.left;
        nHeight = rect.bottom - rect.top;
        nSquareWidth = nWidth / 10;
        nSquareHeight = nHeight / 10;

        for (int i = 0, y = 0; i < floorf(nHeight); i += nSquareHeight, ++y)
        {
            for (int j = 0, x = 0; j < floorf(nWidth); j += nSquareWidth, ++x)
            {
                //new Button(std::wstring(chTable).c_str(), m_hWnd, j, i, nSquareWidth, nSquareHeight, ID_BUTTON_1);
                vecvecBtn[y][x]->Move(j, i, nSquareWidth, nSquareHeight, TRUE);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    }
    return 0;
};