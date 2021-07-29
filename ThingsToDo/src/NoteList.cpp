#include "NoteList.h"

NoteList::NoteList(
    DWORD cCount,
    LPCWSTR* pszColumnDesc,
    DWORD* pdwColumnWidth,
    HWND hParent,
    int iId,
    DWORD dwStyle) :
    cColumns(cCount),
    cRows(0),
    iId(iId),
    pszColumnDesc(pszColumnDesc),
    pdwColumnWidth(pdwColumnWidth)
{

}

BOOL NoteList::Init(HWND hParent)
{
    hWnd = GetDlgItem(hParent, iId);
    InitColumns();

    return hWnd == INVALID_HANDLE_VALUE ? false : true;
}

void NoteList::InitColumns()
{
    SetWindowLongPtr(hWnd, GWLP_USERDATA, cColumns);

    for (DWORD dwIndex = 0; dwIndex < cColumns; dwIndex++) {
        LVCOLUMN lvc;

        ZeroStruct(lvc);

        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvc.cx = pdwColumnWidth[dwIndex];
        lvc.pszText = const_cast<LPWSTR>(pszColumnDesc[dwIndex]);

        ListView_InsertColumn(hWnd, dwIndex, &lvc);

        aCols.push_back(pszColumnDesc[dwIndex]);
    }
}

void NoteList::SetStyle(DWORD dwView)
{
    ListView_SetExtendedListViewStyle(hWnd, dwView);
}

BOOL NoteList::OnNotify(LPARAM lParam)
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

        PostMessage(GetParent(hWnd), NF_MSG, 0x1401 /* Main list */, dwSelected);
        break;

    case NM_RETURN:

        ListView_EditLabel(hWnd, dwSelected);
        break;
    }

    return 0;
}

BOOL NoteList::UpdateList()
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

void NoteList::DeleteItem()
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

void NoteList::Clear()
{
    ListView_DeleteAllItems(hWnd);
    aRows.clear();
    cRows = 0;
}

NoteList::operator HWND()
{
    return hWnd ? hWnd : throw std::runtime_error("Error: control wasn't assigned to the window");
}
