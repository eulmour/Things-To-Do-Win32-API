#pragma once
#include "BaseWindow.h"
#define ID_MAIN_LIST (WM_USER + 0x1001)
#define ID_BUTTON_1 (WM_USER + 0x1011)

class MainWindow : public BaseWindow<MainWindow> {
public:

    MainWindow(WindowClass*& wc)
    {
        Register(wc);
    }

    LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    //VOID SendApcRequest(PAPC_REQUEST pRequest)
    //{
    //    QueueUserAPC(ApcCallback, sInterface.hMainThread, (ULONG_PTR)pRequest);
    //}
    // 
    //VOID NTAPI ApcCallback(ULONG_PTR Parameter);
protected:
    void InitWindowControls();
    void InitFileHierarchy();
    void ResizeList();
    void SetTitle(LPCWSTR lpText);
    HRESULT _WriteComplexToFile();
    HRESULT _ReadComplexFromFile();

    std::wstring sCurrentFilePath;

    DWORD m_dwLastError = 0UL;
    HANDLE m_hFile = nullptr;
private:

    // Controls initialization
    //Button button1 = { L"Press the button", { 20, 140, 100, 20 }, ID_BUTTON_1 };
    //Static label1 = { L"Some text", { 20, 160, 100, 20 } };

    DWORD   listViewStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS;
    LPCWSTR pszColumnNames[2] = { L"Note", L"Created" };
    DWORD   pdwColumnsWidth[2] = { 700, 120 };
    ListView listView = { { 0, 0, 516, 559 }, 2, pszColumnNames, pdwColumnsWidth, ID_MAIN_LIST, listViewStyle };
};
