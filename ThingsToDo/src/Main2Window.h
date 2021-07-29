#pragma once
#include "WinapiBase/Dialog.h"
#include "NoteList.h"

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
    HRESULT WriteDataToFile();
    HRESULT ReadDataFromFile();

    std::wstring sCurrentFilePath;

    HANDLE hFile = nullptr;

private:
    DWORD   listViewStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS;
    LPCWSTR pszColumnNames[2] = { L"Note", L"Modified" };
    DWORD   pdwColumnsWidth[2] = { 700, 120 };
    NoteList listView = { 2, pszColumnNames, pdwColumnsWidth, hDlg, IDC_LIST_ENTRIES, listViewStyle };
};

