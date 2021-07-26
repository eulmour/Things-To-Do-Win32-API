#pragma once
#include "BaseWindow.h"
#define ID_MAIN_LIST (WM_USER + 0x1001)

class MainWindow : public BaseWindow<MainWindow> {
public:

    MainWindow(WindowClass*& wc)
    {
        Register(wc);
    }

    LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

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
};
