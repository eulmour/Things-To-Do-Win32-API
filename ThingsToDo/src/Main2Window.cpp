#include "Main2Window.h"

INT_PTR CALLBACK Main2Window::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (uMsg)
    {
    case WM_INITDIALOG:

        SetMenu(hWnd, LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_THINGSTODO)));
        return (INT_PTR)TRUE;

    case WM_COMMAND:

        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            PostQuitMessage(0);
            EndDialog(hWnd, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);

        return (INT_PTR)TRUE;
    }
        break;

    case WM_CLOSE:

        PostQuitMessage(0);
        EndDialog(hWnd, NULL);

        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}

void Main2Window::InitWindowControls()
{
}

void Main2Window::InitFileHierarchy()
{
}

void Main2Window::ResizeList()
{
}

void Main2Window::SetTitle(LPCWSTR lpText)
{
}

HRESULT Main2Window::_WriteComplexToFile()
{
    return E_NOTIMPL;
}

HRESULT Main2Window::_ReadComplexFromFile()
{
    return E_NOTIMPL;
}
