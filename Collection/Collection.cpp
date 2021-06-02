// Collection.cpp : Defines the entry point for the application.
//
#include "Collection.h"
#include "MainWindow.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    Instance instance(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    WindowClass* mainWindowClass = new WindowClass(instance);
    mainWindowClass->SetClassName(IDC_COLLECTION);
    mainWindowClass->SetCursor(IDC_ARROW);
    mainWindowClass->SetIcon(IDI_COLLECTION);
    mainWindowClass->SetIconSmall(IDI_SMALL);
    mainWindowClass->SetMenu(IDC_COLLECTION);

    MainWindow* mainWindow = new MainWindow(mainWindowClass);
    mainWindow->Create(IDS_APP_TITLE, 300, 200, 516, 559, WS_OVERLAPPEDWINDOW);

    //ListView* listView = new ListView();
    //mainWindow->Append(listView, FILL_CONTENT, FILL_CONTENT, );

    //WindowClass* secondWindowClass = new WindowClass(instance);
    //secondWindowClass->SetClassName(L"Window2");
    //SecondWindow* Window2 = new SecondWindow(secondWindowClass);
    //Window2->Create(IDS_APP_TITLE, 350, 300, 800, 600, WS_OVERLAPPEDWINDOW);

    // TODO: load accelerators in the main window class instead of using this raw way at the moment
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COLLECTION));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
