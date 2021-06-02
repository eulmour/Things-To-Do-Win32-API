#pragma once

#include "framework.h"
#include "resource.h"
//#include <string>

struct Instance
{
    Instance(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
        : hInstance(hInstance),
        hPrevInstance(hPrevInstance),
        lpCmdLine(lpCmdLine),
        nCmdShow(nCmdShow) {}

    HINSTANCE hInstance , hPrevInstance;
    LPWSTR lpCmdLine;
    int nCmdShow;
};

class IWindowClass
{
public:
    //virtual ~IWindowClass() = 0;
    virtual void SetClassName(int) = 0;
    virtual void SetProc(WNDPROC) = 0;
    virtual ATOM Register() = 0;

    Instance* instance = nullptr;
};

//class WindowClass : public IWindowClass
//{
//public:
//    WindowClass(Instance& instance) : wc({ 0 })
//    {
//        wc.hInstance = instance.hInstance;
//    }
//    void SetClassName(int resource) override
//    {
//        LoadString(GetModuleHandle(NULL), resource, m_sClassNameBuffer, 100);
//        wc.lpszClassName = m_sClassNameBuffer;
//    }
//    void SetClassName(LPCWSTR sName) { wc.lpszClassName = sName; }
//    void SetProc(WNDPROC ptr) override { wc.lpfnWndProc = ptr; }
//    ATOM Register() override { return RegisterClass(&wc); }
//    
//    WNDCLASS wc;
//};

class WindowClass : public IWindowClass{
public:

    WindowClass(Instance& instance) : wcex({ 0 })
    {
        //Default class properties
        wcex.cbSize =       sizeof(WNDCLASSEX);
        wcex.style =        CS_HREDRAW | CS_VREDRAW;
        wcex.cbClsExtra =   0;
        wcex.cbWndExtra =   0;
        wcex.hInstance =    instance.hInstance;

        //Must be overwritten
        wcex.hIcon =            LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_COLLECTION));
        wcex.hCursor =          LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground =    (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName =     MAKEINTRESOURCEW(IDC_COLLECTION);
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
        this->instance = &instance;
    }

    void SetBackground(HBRUSH brush)    { wcex.hbrBackground = brush; }
    void SetClassName(int resource) override
    {
        LoadString(GetModuleHandle(NULL), resource, m_sClassNameBuffer, 100);
        wcex.lpszClassName = m_sClassNameBuffer;
    }
    void SetClassName(LPCWSTR sName) { wcex.lpszClassName = sName; }
    void SetCursor(LPWSTR resource)     { wcex.hCursor = LoadCursor(nullptr, resource); }
    void SetIcon(int resource)          { wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(resource)); }
    void SetIconSmall(int resource)     { wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(resource)); }
    void SetMenu(int resource)          { wcex.lpszMenuName = MAKEINTRESOURCE(resource); }
    void SetProc(WNDPROC ptr) override  { wcex.lpfnWndProc = ptr; }
    virtual ATOM Register() override    { return RegisterClassEx(&wcex); }

    TCHAR m_sClassNameBuffer[100];
    TCHAR m_sMenuNameBuffer[100] = L"";
    WNDCLASSEX wcex;
};

class IControl
{
public:
    //virtual IControl* Create(HWND) = 0;
    virtual BOOL Show(BOOL bShow) { return ShowWindow(hWnd, bShow); }
    virtual BOOL Move(int x, int y, int nWidth, int nHeight, BOOL bRepaint) { return MoveWindow(hWnd, x, y, nWidth, nHeight, bRepaint); }
    virtual BOOL Enable(BOOL bEnable) { return EnableWindow(hWnd, bEnable); }
    virtual HWND Window() { return hWnd; }
    virtual operator HWND() { return this->hWnd; };
protected:
    HWND hWnd = nullptr;
    HMENU hMenu = { 0 };
    HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
};

class Static : public IControl
{
public:
    Static(LPCWSTR lpWindowName, HWND hParent, int x, int y, int nWidth, int nHeight, WORD nId = 0, DWORD dwStyle = WS_CHILD | WS_VISIBLE)
    {
        hWnd = CreateWindow(TEXT("Static"), lpWindowName, dwStyle, x, y, nWidth, nHeight, hParent, (HMENU)nId, nullptr, nullptr);
        SendMessage(hWnd, WM_SETFONT, WPARAM(font), TRUE); // Set default font
    }
};

class Button : public IControl
{
public:
    Button(LPCWSTR lpWindowName, HWND hParent, int x, int y, int nWidth, int nHeight, WORD nId = 0, DWORD dwStyle = WS_CHILD | WS_VISIBLE)
    {
        hWnd = CreateWindow(TEXT("Button"), lpWindowName, dwStyle, x, y, nWidth, nHeight, hParent, (HMENU)nId, nullptr, nullptr);
        SendMessage(hWnd, WM_SETFONT, WPARAM(font), TRUE); // Set default font
    }
};
