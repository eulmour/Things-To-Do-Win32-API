#pragma once
#include <framework.h>
#include "resource.h"

template <class DERIVED_TYPE>
class BaseDialog
{
protected:

    virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    static inline INT_PTR DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        static DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_INITDIALOG)
        {
            pThis = reinterpret_cast<DERIVED_TYPE*>(lParam);
            pThis->hDlg = hDlg;
        }

        if (pThis)
        {
            return pThis->OnMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hDlg, uMsg, wParam, lParam);
        }
    }

public:
    BaseDialog() : hDlg(), iId() { }

    constexpr void Create()
    {
        hDlg = CreateDialogParamW(
            GetModuleHandle(NULL),
            MAKEINTRESOURCE(iId),
            NULL,
            (DLGPROC)DERIVED_TYPE::DialogProc,
            (LPARAM)reinterpret_cast<DERIVED_TYPE*>(this));

        ShowWindow(hDlg, 10);
    }

	HWND hDlg;
    int iId;
};
