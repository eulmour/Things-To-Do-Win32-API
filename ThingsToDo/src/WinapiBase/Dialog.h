#pragma once
#include <framework.h>

template <class DERIVED_TYPE>
class BaseDialog
{
protected:

	HWND hWnd;
    virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    static INT_PTR WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_INITDIALOG)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->hWnd = hWnd;
        }
        else
        {
            pThis = (DERIVED_TYPE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
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
    BaseDialog() : m_hWnd(nullptr) { }

    BOOL Create(int resource)
    {
        DialogBox(m_pClass->instance->hInstance, MAKEINTRESOURCE(resource), GetParent(hWnd), DERIVED_TYPE::OnMessage);
    }
};
