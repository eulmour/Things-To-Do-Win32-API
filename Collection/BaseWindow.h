#pragma once
#include "Base.h"

template <class DERIVED_TYPE>
class BaseDialog
{
protected:
    HWND m_hWnd;
    WindowClass* m_pClass;
    virtual INT_PTR OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    void Register(WindowClass* pClass) { m_pClass = pClass; }

    static INT_PTR WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_INITDIALOG)
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
    BaseDialog() : m_hWnd(nullptr) { }
    BOOL Create(int resource)
    {
        DialogBox(m_pClass->instance->hInstance, MAKEINTRESOURCE(resource), GetParent(m_hWnd), DERIVED_TYPE::OnMessage);
    }
};

template <class DERIVED_TYPE>
class BaseWindow
{
protected:

    HWND m_hWnd;
    WindowClass* m_pClass;
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
    BaseWindow() : m_hWnd(nullptr), m_pClass(nullptr) { }

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

        //MessageBox(m_hWnd, GetErrorMessage(GetLastError()).c_str(), TEXT("Warning"), MB_OK | MB_ICONINFORMATION);

        ShowWindow(m_hWnd, m_pClass->instance->nCmdShow);
        UpdateWindow(m_hWnd);

        return (m_hWnd ? TRUE : FALSE);
    }

    HWND Window() const { return m_hWnd; }
    std::basic_string<TCHAR> GetErrorMessage(DWORD dwErrorCode)
    {
        LPTSTR psz = NULL;
        const DWORD cchMsg = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS
            | FORMAT_MESSAGE_ALLOCATE_BUFFER,
            NULL, // (not used with FORMAT_MESSAGE_FROM_SYSTEM)
            dwErrorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPTSTR>(&psz),
            0,
            NULL);
        if (cchMsg > 0)
        {
            // Assign buffer to smart pointer with custom deleter so that memory gets released
            // in case String's c'tor throws an exception.
            auto deleter = [](void* p) { ::HeapFree(::GetProcessHeap(), 0, p); };
            std::unique_ptr<TCHAR, decltype(deleter)> ptrBuffer(psz, deleter);
            return std::basic_string<TCHAR>(ptrBuffer.get(), cchMsg);
        }
        else
        {
            throw std::runtime_error("Failed to retrieve error message string.");
        }
    }
    INT_PTR Dialog(int resource, DLGPROC ptr)
    {
        return DialogBox(m_pClass->instance->hInstance, MAKEINTRESOURCE(resource), m_hWnd, ptr);
    }

    INT_PTR Box(int resource, DLGPROC ptr)
    {
        return CreateDialog(m_pClass->instance->hInstance, MAKEINTRESOURCE(resource), m_hWnd, ptr);
    }
};
