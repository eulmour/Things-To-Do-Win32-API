#pragma once

#include "framework.h"
#include "resource.h"

#define LM_SELECTALL (WM_USER + 0x0300)

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
    virtual ~IWindowClass() {}
    virtual void SetClassName(int) = 0;
    virtual void SetProc(WNDPROC) = 0;
    virtual ATOM Register() = 0;

    Instance* instance = nullptr;
};

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
        wcex.hIcon =            LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_THINGSTODO));
        wcex.hCursor =          LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground =    (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName =     MAKEINTRESOURCEW(IDC_THINGSTODO);
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
    IControl(LPCWSTR lpClassName, LPCWSTR lpWindowName, Position pos, WORD wId, DWORD dwStyle)
        : sClassName(lpClassName), sText(lpWindowName), m_Pos(pos), wId(wId), dwStyle(dwStyle)
    {
    }

    virtual ~IControl() {}

    virtual void Create(HWND hParent)
    {
        hWnd = CreateWindow(sClassName.c_str(), sText.c_str(), dwStyle, m_Pos.x, m_Pos.y, m_Pos.width, m_Pos.height, hParent, (HMENU)wId, nullptr, nullptr);
        //SendMessage(hWnd, WM_SETFONT, WPARAM(font), TRUE); // Set default font
    }

    virtual void Post(HWND hParent) {}
    virtual BOOL Show(BOOL bShow) { return ShowWindow(hWnd, bShow); }
    virtual BOOL Enable(BOOL bEnable) { return EnableWindow(hWnd, bEnable); }

    virtual BOOL Move(int x, int y, int nWidth, int nHeight, BOOL bRepaint)
    {
        return MoveWindow(hWnd, x, y, nWidth, nHeight, bRepaint);
    }

    virtual HWND Window()
    {
        return hWnd ? hWnd : throw std::runtime_error("Error: control wasn't assigned to the window");
    }
    
    operator HWND()
    {
        return hWnd ? hWnd : throw std::runtime_error("Error: control wasn't assigned to the window");
    };

protected:
    std::wstring    sClassName, sText;
    Position        m_Pos;
    WORD            wId = { 0 };
    DWORD           dwStyle;
    HWND            hWnd = nullptr;
    HFONT           font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
};

class Static : public IControl
{
public:
    Static(LPCWSTR lpWindowName, Position pos, WORD wId = 0, DWORD dwStyle = WS_CHILD | WS_VISIBLE)
        : IControl(L"Static", lpWindowName, pos, wId, dwStyle)
    {
    }
};

class Button : public IControl
{
public:
    Button(LPCWSTR lpWindowName, Position pos, WORD wId = 0, DWORD dwStyle = WS_CHILD | WS_VISIBLE)
        : IControl(L"Button", lpWindowName, pos, wId, dwStyle)
    {
    }
};

class ListView : public IControl
{
public:
    ListView(
        Position    pos,
        DWORD       cCount,
        LPCWSTR*    pszColumnDesc,
        DWORD*      pdwColumnWidth,
        WORD        wId = NULL,
        DWORD       dwStyle = WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | WS_BORDER)
      : IControl(L"SysListView32", L"", pos, wId, dwStyle),
        cColumns(cCount),
        cRows(0),
        pszColumnDesc(pszColumnDesc),
        pdwColumnWidth(pdwColumnWidth)
    {
    }

    void Post(HWND hParent) override
    {
        SetWindowLongPtr(hWnd, GWLP_USERDATA, cColumns);

        for (DWORD dwIndex = 0; dwIndex < cColumns; dwIndex++) {
            LVCOLUMN lvc;

            ZeroStruct(lvc);

            lvc.mask    = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            lvc.cx      = pdwColumnWidth[dwIndex];
            lvc.pszText = const_cast<LPWSTR>(pszColumnDesc[dwIndex]);

            ListView_InsertColumn(hWnd, dwIndex, &lvc);

            aCols.push_back(pszColumnDesc[dwIndex]);
        }
    }

    void SetStyle(DWORD dwView)
    {
        ListView_SetExtendedListViewStyle(hWnd, dwView);
    }

    BOOL OnNotify(LPARAM lParam)
    {
        NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
        
        switch (((LPNMHDR)lParam)->code)
        {

        case LVN_GETDISPINFO:

            // sets text to a ListView field
            plvdi->item.pszText = (LPWSTR)aRows[plvdi->item.iItem].aTextFields[plvdi->item.iSubItem].c_str();
            break;

        case LVN_BEGINLABELEDIT:

            m_hEditLabel = ListView_GetEditControl(hWnd);
            break;

        case LVN_ENDLABELEDIT:
        {
            TCHAR temp_buffer[256] = L"";

            if (!m_bPreventEdit)
            {
                plvdi->item.iSubItem = 0;     // we get the item only (change for sub)
                plvdi->item.pszText = temp_buffer;   // text type

                //TODO: potentially rough patch of code
                GetWindowTextW(m_hEditLabel, temp_buffer, sizeof(temp_buffer) / sizeof(wchar_t)); // get the text into a buffer
                this->aRows[dwSelected].aTextFields[0] = temp_buffer;
            }
            m_bPreventEdit = false;
        }
        break;

        case LVN_ITEMCHANGED:
        {
            LPNMLISTVIEW pNMLV = (LPNMLISTVIEW)lParam;
            dwSelected = ListView_GetNextItem(hWnd, -1, LVNI_FOCUSED);

            if (pNMLV->uChanged & LVIF_STATE && pNMLV->uOldState)
            {
                switch (pNMLV->uNewState & LVIS_STATEIMAGEMASK)
                {
                case 0x2000: /*checked*/
                    aRows[pNMLV->iItem].bChecked = true;
                    break;
                case 0x1000: /*unchecked*/
                    aRows[pNMLV->iItem].bChecked = false;
                    break;
                }
            }
        }
        break;

        case LVM_DELETEALLITEMS:

            ListView_DeleteAllItems(hWnd);
            aRows.clear();
            cRows = 0;
            break;

        case LM_SELECTALL:

            for (int i = 0; i < cRows; ++i)
            {
                ListView_SetItemState(hWnd, i, LVIS_SELECTED, 0x00f);
            }

            break;

        case NM_DBLCLK:

            PostMessage(GetParent(hWnd), NF_MSG , 0x1401 /* Main list */, dwSelected);
            break;

        case NM_RETURN:

            ListView_EditLabel(hWnd, dwSelected);
            break;
        }

        return 0;
    }

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

        //SendApcRequest()
    }

    BOOL UpdateList()
    {
        LVITEM lvI;

        lvI.pszText = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
        lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
        lvI.iSubItem = 0;
        lvI.state = 0;

        int iNewRowCount = static_cast<int>(aRows.size());

        for (int index = this->cRows; index < iNewRowCount; ++index)
        {
            lvI.iItem = index;
            lvI.iImage = index;

            // Insert items into the list.
            if (ListView_InsertItem(hWnd, &lvI) == -1)
                return FALSE;
            // Set checkbox checked
            ListView_SetCheckState(hWnd, index, aRows[index].bChecked);
        }

        this->cRows = iNewRowCount;

        return TRUE;
    }

    void DeleteItem()
    {
        while (true)
        {
            dwSelected = ListView_GetNextItem(hWnd, -1, LVNI_SELECTED);

            if (dwSelected == -1)
                return;

            aRows.erase(aRows.begin() + dwSelected);
            ListView_DeleteItem(hWnd, dwSelected);
        }
    }

    void Clear()
    {
        ListView_DeleteAllItems(hWnd);
        aRows.clear();
        cRows = 0;
    }

    WORD GetId() { return wId; }
    std::vector<ListViewRow>& GetData() { return aRows; }
    std::vector<std::wstring>& GetHeader() { return aCols; }

private:

    DWORD cColumns, cRows;
    LPCWSTR* pszColumnDesc;
    DWORD* pdwColumnWidth;
    std::vector<ListViewRow> aRows;
    std::vector<std::wstring> aCols;

    LONG dwSelected = -1;
    LONG dwOpSelected = -1;
    HWND m_hEditLabel = nullptr;
    BOOL m_bPreventEdit = false;

    static VOID NTAPI ApcCallback(ULONG_PTR Parameter)
    {
        //TODO 123123123
    }

    void SendApcRequest(ListViewRow* pRow)
    {
        HWND hMainThread = static_cast<HWND>(OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId()));
        if (hMainThread && hMainThread != INVALID_HANDLE_VALUE)
            QueueUserAPC(ApcCallback, hMainThread, (ULONG_PTR)pRow);
    }
};