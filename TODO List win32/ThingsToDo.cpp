#include "framework.h"
#include "ThingsToDo.h"

#include "MainWindow.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TODOLISTWIN32));

    int iUserScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int iUserScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    int iUserWindowWidth = iUserScreenWidth / 3;
    int iUserWindowHeight = iUserScreenHeight / 2;

    MainWindow mainWindow;
    if (!mainWindow.CreateEx(L"Things To Do", WS_OVERLAPPEDWINDOW /*WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX*/, 0UL,
        (iUserScreenWidth / 2) - (iUserWindowWidth / 2), (iUserScreenHeight / 2) - (iUserWindowHeight / 2),
        iUserWindowWidth, iUserWindowHeight, (HWND)nullptr, (HMENU)nullptr, hInstance))
        return FALSE;

    mainWindow.Show(nCmdShow);
    mainWindow.Update();

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(mainWindow.m_hWnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
