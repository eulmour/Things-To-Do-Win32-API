// Collection.cpp : Defines the entry point for the application.
//
#include "ThingsToDo.h"

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

    Button button1(L"Press the button", { 20, 140, 100, 20 }, ID_BUTTON_1);
    mainWindow->Append(&button1);

    Static label1(L"Some text", { 20, 160, 100, 20 });
    mainWindow->Append(&label1);

    DWORD   listViewStyle       = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | WS_BORDER;
    LPCWSTR pszColumnNames[]    = { L"Address", L"Previous Value", L"New Value", L"Change Time" };
    DWORD   pdwColumnsWidth[]   = { 115, 115, 115, 85 };

    ListView listView({ 0, 0, 516, 559 }, 4, pszColumnNames, pdwColumnsWidth, ID_MAIN_LIST, listViewStyle);
    mainWindow->AppendList(&listView, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

    listView.AddRow<0xFFFFFF, TRUE>(L"First col", L"Second col", L"Third col", L"Fourth col");
    listView.AddRow<0xFFFFFF, TRUE>(L"First col", L"Second col", L"Third col", L"Fourth col");
    listView.UpdateList();

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
