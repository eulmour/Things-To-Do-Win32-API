#pragma once
#include "BaseWindow.h"

class MainWindow : public BaseWindow<MainWindow> {
public:

    MainWindow(WindowClass*& wc)
    {
        Register(wc);
    }

    LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};