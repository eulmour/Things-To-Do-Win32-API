#pragma once
#include "Base.h"

template <class DERIVED_TYPE>
class BaseWindow
{
protected:

    HWND m_hWnd, m_hMainThread;
    WindowClass* m_pClass;
    std::vector<ListView*> m_aListViewList;

    virtual LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    void Register(WindowClass* pClass) { m_pClass = pClass; }

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->m_hWnd = hWnd;
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
    BaseWindow() : m_hWnd(nullptr), m_hMainThread(nullptr), m_pClass(nullptr) { }

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
        m_pClass->SetProc(DERIVED_TYPE::WindowProc);
        m_pClass->Register();

        std::unique_ptr<TCHAR[]> pWindowTitle(new TCHAR[100 * sizeof(TCHAR)]);
        LoadString(GetModuleHandle(NULL), nTitleId, pWindowTitle.get(), 100);

        m_hWnd = CreateWindowExW
        (
            dwExStyle,
            m_pClass->wcex.lpszClassName,
            pWindowTitle.get(),
            dwStyle,
            x, y,
            nWidth, nHeight,
            hWndParent,
            (HMENU)0,
            m_pClass->wcex.hInstance,
            this
        );

        m_hMainThread = static_cast<HWND>(OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId()));

        //MessageBox(m_hWnd, GetErrorMessage(GetLastError()).c_str(), TEXT("Warning"), MB_OK | MB_ICONINFORMATION);

        ShowWindow(m_hWnd, m_pClass->instance->nCmdShow);
        UpdateWindow(m_hWnd);

        return (m_hWnd ? TRUE : FALSE);
    }

    HWND Window() const { return m_hWnd; }

    INT_PTR Dialog(int resource, DLGPROC ptr, LPARAM lParam = 0L)
    {
        return DialogBoxParam(m_pClass->instance->hInstance, MAKEINTRESOURCE(resource), m_hWnd, ptr, lParam);
    }

    INT_PTR Box(int resource, DLGPROC ptr)
    {
        return CreateDialog(m_pClass->instance->hInstance, MAKEINTRESOURCE(resource), m_hWnd, ptr);
    }

    void Append(auto* const control)
    {
        control->Create(this->m_hWnd);
    }

    void AppendList(auto* const control, DWORD dwView = NULL)
    {
        this->m_aListViewList.push_back(control);
        control->Create(this->m_hWnd);
        control->SetStyle(dwView);
        control->Post(m_hWnd);
    }
};
