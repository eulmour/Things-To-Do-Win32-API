#pragma once
#include "framework.h"
#include "resource.h"

#define LM_SELECTALL (WM_USER + 0x0300)

class TaskList
{
public:

    TaskList(
        DWORD       cCount,
        LPCWSTR*    pszColumnDesc,
        DWORD*      pdwColumnWidth,
        HWND        hParent,
        int         iId = NULL);

    BOOL OnNotify(LPARAM lParam);

    BOOL Init(HWND hParent);
    void InitColumns();
    void SetStyle(DWORD dwView);
    BOOL UpdateList();
    void DeleteItem();
    void Clear();

    int GetId() const { return iId; }
    std::vector<ListViewRow>& GetData() { return aRows; }
    std::vector<std::wstring>& GetHeader() { return aCols; }
    operator HWND();

    template <DWORD dwColor, BOOL bChecked, typename ... Param>
    void AddRow(Param ... params)
    {
        static_assert(are_same<Param...>::value, "Error: type mismatch");

        if (sizeof...(Param) != cColumns)
            throw std::runtime_error("Runtime error: wrong argument number");

        aRows.emplace_back(bChecked, dwColor, std::vector<std::wstring>());

        for (auto&& x : { params... })
        {
            aRows.back().aTextFields.push_back(x);
        }
    }

    LONG dwSelected = -1;

private:

    HWND hWnd, hParent;
    int iId;
    DWORD cColumns, cRows;
    LPCWSTR* pszColumnDesc;
    DWORD* pdwColumnWidth;
    std::vector<ListViewRow> aRows;
    std::vector<std::wstring> aCols;

    HWND m_hEditLabel = nullptr;
    BOOL m_bPreventEdit = false;
};

