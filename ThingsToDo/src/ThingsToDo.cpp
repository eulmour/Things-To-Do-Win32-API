// ThingsToDo.cpp : Defines the entry point for the application.

#include "ThingsToDo.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    Instance instance(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    WindowClass* mainWindowClass = new WindowClass(instance);
    mainWindowClass->SetClassName(IDC_THINGSTODO);
    mainWindowClass->SetCursor(IDC_ARROW);
    mainWindowClass->SetIcon(IDI_THINGSTODO);
    mainWindowClass->SetIconSmall(IDI_SMALL);
    mainWindowClass->SetMenu(IDC_THINGSTODO);

    int iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int iScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    int iWindowWidth = iScreenWidth / 3;
    int iWindowHeight = iScreenHeight / 2;
    int iWindowLeft = (iScreenWidth / 2) - (iWindowWidth / 2);
    int iWindowTop = (iScreenHeight / 2) - (iWindowHeight / 2);

    MainWindow* mainWindow = new MainWindow(mainWindowClass);
    mainWindow->Create(IDS_APP_TITLE, iWindowLeft, iWindowTop, iWindowWidth, iWindowHeight, WS_OVERLAPPEDWINDOW);

    MSG msg;
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_THINGSTODO));

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(mainWindow->Window(), hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
