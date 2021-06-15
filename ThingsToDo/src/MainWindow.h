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
};