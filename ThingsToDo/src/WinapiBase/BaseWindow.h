#pragma once
#include "Base.h"

template <class DERIVED_TYPE>
class BaseWindow
{
protected:

    HWND hWnd;
    WindowClass* pClass;
    std::vector<ListView*> aListViewList;

    virtual LRESULT OnMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    void Register(WindowClass* pClass) { this->pClass = pClass; }

    static inline LRESULT CALLBACK WindowProc(HWND hHandle, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        static DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
            pThis->hWnd = hHandle;
        }
        if (pThis)
        {
            return pThis->OnMessage(hHandle, uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hHandle, uMsg, wParam, lParam);
        }
    }

public:
    BaseWindow() : hWnd(nullptr), pClass(nullptr) { }

    BOOL Create(
        int nTitleId,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        DWORD dwStyle = WS_OVERLAPPEDWINDOW,
        DWORD dwExStyle = 0,
        HWND hWndParent = nullptr
    )
    {
        pClass->SetProc(DERIVED_TYPE::WindowProc);
        pClass->Register();

        std::unique_ptr<TCHAR[]> pWindowTitle(new TCHAR[100 * sizeof(TCHAR)]);
        LoadString(GetModuleHandle(NULL), nTitleId, pWindowTitle.get(), 100);

        hWnd = CreateWindowExW
        (
            dwExStyle,
            pClass->wcex.lpszClassName,
            pWindowTitle.get(),
            dwStyle,
            x, y,
            nWidth, nHeight,
            hWndParent,
            (HMENU)0,
            pClass->wcex.hInstance,
            this
        );

        ShowWindow(hWnd, pClass->instance->nCmdShow);
        UpdateWindow(hWnd);

        return (hWnd ? TRUE : FALSE);
    }

    HWND Window() const { return hWnd; }

    INT_PTR Dialog(int resource, DLGPROC ptr, LPARAM lParam = 0L)
    {
        return DialogBoxParam(pClass->instance->hInstance, MAKEINTRESOURCE(resource), hWnd, ptr, lParam);
    }

    INT_PTR Box(int resource, DLGPROC ptr)
    {
        return CreateDialog(pClass->instance->hInstance, MAKEINTRESOURCE(resource), hWnd, ptr);
    }

    void Append(auto* const control)
    {
        control->Create(this->hWnd);
    }

    void AppendList(auto* const control, DWORD dwView = NULL)
    {
        this->aListViewList.push_back(control);
        control->Create(this->hWnd);
        control->SetStyle(dwView);
        control->Post(hWnd);
    }
};
