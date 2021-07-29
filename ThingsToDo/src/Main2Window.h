#pragma once
#include "WinapiBase/Dialog.h"
#include "NoteList.h"

class Main2Window : public BaseDialog<Main2Window>
{
public:

    Main2Window(int id) { iId = id; }
    INT_PTR CALLBACK OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:

    void InitWindowControls();
    void InitFileHierarchy();
    void ResizeList();
    void SetTitle(LPCWSTR lpText);
    HRESULT WriteDataToFile();
    HRESULT ReadDataFromFile();

    std::wstring sCurrentFilePath;

    HANDLE hFile = nullptr;

private:
    LPCWSTR pszColumnNames[2] = { L"Note", L"Modified" };
    DWORD   pdwColumnsWidth[2] = { 700, 120 };
    NoteList noteList = { 2, pszColumnNames, pdwColumnsWidth, hDlg, IDC_LIST_ENTRIES };
};

