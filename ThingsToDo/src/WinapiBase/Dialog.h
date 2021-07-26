#pragma once
#include <framework.h>
#include "resource.h"

template <class DERIVED_TYPE>
class BaseDialog
{
protected:

    virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    static inline INT_PTR WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        static DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_INITDIALOG)
        {
            pThis = reinterpret_cast<DERIVED_TYPE*>(lParam);
            pThis->hWnd = hWnd;
        }

        if (pThis)
        {
            return pThis->OnMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

public:
    BaseDialog() : hWnd(), iId() { }

    constexpr void Create()
    {
        hWnd = CreateDialogParamW(
            GetModuleHandle(NULL),
            MAKEINTRESOURCE(iId),
            NULL,
            DERIVED_TYPE::WindowProc,
            (LPARAM)reinterpret_cast<DERIVED_TYPE*>(this));

        ShowWindow(hWnd, 10);
    }

	HWND hWnd;
    int iId;
};
