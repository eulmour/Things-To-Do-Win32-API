#pragma once
#include "WinapiBase/Dialog.h"

class Main2Window : public BaseDialog<Main2Window>
{
public:

    Main2Window(int id) { iId = id; }
    INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:

    void InitWindowControls();
    void InitFileHierarchy();
    void ResizeList();
    void SetTitle(LPCWSTR lpText);
    HRESULT _WriteComplexToFile();
    HRESULT _ReadComplexFromFile();

    std::wstring sCurrentFilePath;

    HANDLE hFile = nullptr;
};

