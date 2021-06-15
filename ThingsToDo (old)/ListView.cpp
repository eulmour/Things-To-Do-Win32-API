#include "ListView.h"
#include <tchar.h>
#include <Shlobj.h>
#include "resource.h"
#include <atlbase.h>
//--- TODO: Avoid define repetition
#define LM_DO_FINDFIRST (WM_USER + 0x0300)
#define LM_DO_EDITCANCEL (WM_USER + 0x0301)

wwt::ListView::ListView(std::initializer_list<Column> cols)
{
	m_vColumns.insert(m_vColumns.begin(), cols.begin(), cols.end());
}

BOOL wwt::ListView::Initialize()
{

    LVCOLUMN lvColumn = { 0 };
    ListView_DeleteAllItems(m_hWnd);

    // Initialize the LVCOLUMN structure.
    // The mask specifies that the format, width, text,
    // and subitem members of the structure are valid.
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;
    // Add the columns.
    for (int iC = 0; iC < m_vColumns.size(); iC++)
    {
        lvColumn.iSubItem = iC;
        lvColumn.pszText = (LPWSTR)m_vColumns[iC].caption.c_str();
        lvColumn.cx = m_vColumns[iC].size;

        // Insert the columns into the list view.
        if (ListView_InsertColumn(m_hWnd, iC, &lvColumn) == -1)
            return FALSE;
    }

    return TRUE;
}

BOOL wwt::ListView::UpdateCheckBoxState()
{
    for (size_t index = 0; index < m_vColumns[0].items.size(); ++index)
    {
        m_vColumns[1].items[index].second = m_vColumns[0].items[index].second = ListView_GetCheckState(m_hWnd, index);
    }
    return 0;
}

void wwt::ListView::Clear()
{
    ListView_DeleteAllItems(m_hWnd);
    ListView_SetItemCount(m_hWnd, m_nRows);
    this->m_nRows = 0;
    InvalidateRect(m_hWnd, nullptr, TRUE);
}

BOOL wwt::ListView::UpdateList()
{
    //--- This code related to ownerdata mode

    //--- This code works in case when default mode using.
    LVITEM lvI;

    // Initialize LVITEM members that are common to all items.
    lvI.pszText = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
    lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
    lvI.stateMask = 0;
    lvI.iSubItem = 0;
    lvI.state = 0;

    size_t newRowCount = m_vColumns[0].items.size();

    // Initialize LVITEM members that are different for each item.
    for (int index = this->m_nRows; index < newRowCount; ++index)
    {

        lvI.iItem = index;
        lvI.iImage = index;

        // Insert items into the list.
        if (ListView_InsertItem(m_hWnd, &lvI) == -1)
            return FALSE;
        // Set checkbox checked
        ListView_SetCheckState(m_hWnd, index, m_vColumns[0].items[index].second);
    }

    this->m_nRows = newRowCount;
    
    //auto func = []() {};
    //ListView_SortItems(m_hWnd, &func, NULL);
    return TRUE;
}

BOOL wwt::ListView::OnNotify(LPARAM lParam)
{
    NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
    TCHAR temp_buffer[256] = L"";
    TCHAR temp_largebuffer[256] = L"";

    switch (((LPNMHDR)lParam)->code)
    {
    case LVN_GETDISPINFO:
        plvdi->item.pszText = (LPWSTR)m_vColumns[plvdi->item.iSubItem].items[plvdi->item.iItem].first.c_str();
        break;

    case LVN_BEGINLABELEDIT:
        m_hEditLabel = ListView_GetEditControl(m_hWnd);
        break;

    case LVN_ENDLABELEDIT:
        if (!m_bPreventEdit)
        {
            iSelected = ListView_GetNextItem(m_hWnd, -1, LVNI_FOCUSED);  // get selected item
            plvdi->item.iSubItem = 0;     // we get the item only (change for sub)
            plvdi->item.pszText = temp_buffer;   // text type

            //TODO: potentially rough patch of code
            GetWindowTextW(m_hEditLabel, temp_buffer, sizeof(temp_buffer) / sizeof(wchar_t)); // get the text into a buffer
            this->m_vColumns[0].items[iSelected].first = temp_buffer;
        }
        m_bPreventEdit = false;
        break;

    case LVM_DELETEALLITEMS:
        ListView_DeleteAllItems(m_hWnd);
        m_vColumns[0].items.clear();
        m_vColumns[1].items.clear();
        break;

    case NM_DBLCLK:

        iSelected = ListView_GetNextItem(m_hWnd, -1, LVNI_FOCUSED);

        if (iSelected == -1)
        {
            MessageBox(GetParent(m_hWnd), TEXT("No items in list"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);
            break;
        }

        //memset(&plvdi->item, 0, sizeof(plvdi->item));
        //plvdi->item.mask = LVIF_TEXT;
        //plvdi->item.iSubItem = 0;
        //plvdi->item.pszText = (LPWSTR)L"Jopa";
        //plvdi->item.cchTextMax = 256;
        //plvdi->item.iItem = iSelected;

        //SendMessage(m_hWnd, LVM_GETITEMTEXT, iSelected, (LPARAM)&plvdi->item);

        for (size_t i = 0; i < m_vColumns.size(); ++i)
        {
            ListView_GetItemText(m_hWnd, iSelected, static_cast<int>(i), temp_buffer, 256);
            swprintf_s(temp_buffer, L"%s\n", temp_buffer);
            lstrcat(temp_largebuffer, temp_buffer);
        }

        MessageBox(GetParent(m_hWnd), temp_largebuffer, TEXT("Item"), MB_OK);
        break;

    case NM_CLICK:
        iSelected = ListView_GetNextItem(m_hWnd, -1, LVNI_FOCUSED);
        break;

    case NM_RETURN:
        iSelected = ListView_GetNextItem(m_hWnd, -1, LVNI_FOCUSED);
        ListView_EditLabel(m_hWnd, iSelected);
        break;

    case LM_DO_FINDFIRST:
        MessageBeep(MB_OK);
        break;
    case LM_DO_EDITCANCEL:
        m_bPreventEdit = true;
        ListView_CancelEditLabel(m_hEditLabel);
        break;
    }

    return 0;
}

void wwt::ListView::UpdateMenu(HMENU hMenu)
{
    //TODO: this is dummy, only for testing purposes

    //UINT uID = (UINT)GetMenu(m_hWnd);
    //DWORD dwStyle;
    ////uncheck all of these guys
    //CheckMenuItem(hMenu, IDM_ABOUT, MF_BYCOMMAND | MF_UNCHECKED);
    //CheckMenuItem(hMenu, IDM_ABOUT, MF_BYCOMMAND | MF_UNCHECKED);
    //CheckMenuItem(hMenu, IDM_ABOUT, MF_BYCOMMAND | MF_UNCHECKED);
    //CheckMenuItem(hMenu, IDM_ABOUT, MF_BYCOMMAND | MF_UNCHECKED);
    ////check the appropriate view menu item
    //dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);
    //switch (dwStyle & LVS_TYPEMASK)
    //{
    //case LVS_ICON:
    //    uID = IDM_ABOUT;
    //    break;
    //case LVS_SMALLICON:
    //    uID = IDM_ABOUT;
    //    break;
    //case LVS_LIST:
    //    uID = IDM_ABOUT;
    //    break;
    //case LVS_REPORT:
    //    uID = IDM_ABOUT;
    //    break;
    //}
    //CheckMenuRadioItem(hMenu, IDM_ABOUT, IDM_ABOUT, uID, MF_BYCOMMAND | MF_CHECKED);
}

void wwt::ListView::SetView(DWORD dwView)
{   // Retrieve the current window style. 
    DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);

    // Set the window style only if the view bits changed.
    if ((dwStyle & LVS_TYPEMASK) != dwView)
    {
        SetWindowLong(m_hWnd,
            GWL_STYLE,
            (dwStyle & ~LVS_TYPEMASK) | dwView);
    }
}

void wwt::ListView::SetStyle(DWORD dwView)
{
    ListView_SetExtendedListViewStyle(m_hWnd, dwView);
}

void wwt::ListView::HighlightItems(int index) const
{
    ListView_SetItemState(m_hWnd, -1, 0, LVIS_SELECTED); // deselect all items
    ListView_EnsureVisible(m_hWnd, index, FALSE);
    ListView_SetItemState(m_hWnd, index, LVIS_SELECTED, LVIS_SELECTED);
    ListView_SetItemState(m_hWnd, index, LVIS_FOCUSED, LVIS_FOCUSED); // optional
}

void wwt::ListView::PopItem()
{
    if (iSelected == -1)
        return;

    ListView_DeleteItem(m_hWnd, iSelected);
    m_vColumns[0].items.erase(m_vColumns[0].items.begin() + iSelected);
    m_vColumns[1].items.erase(m_vColumns[1].items.begin() + iSelected);
}

BOOL wwt::ListView::InitListViewImageLists(int resource_id)
{
    HICON hiconItem;     // Icon for list-view items.
    HIMAGELIST hLarge;   // Image list for icon view.
    HIMAGELIST hSmall;   // Image list for other views.

    // Create the full-sized icon image lists. 
    hLarge = ImageList_Create(GetSystemMetrics(SM_CXICON),
        GetSystemMetrics(SM_CYICON),
        ILC_MASK, 1, 1);

    hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
        GetSystemMetrics(SM_CYSMICON),
        ILC_MASK, 1, 1);

    // Add an icon to each image list.  
    hiconItem = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(resource_id));

    ImageList_AddIcon(hLarge, hiconItem);
    ImageList_AddIcon(hSmall, hiconItem);

    DestroyIcon(hiconItem);

    // When you are dealing with multiple icons, you can use the previous four lines of 
    // code inside a loop. The following code shows such a loop. The 
    // icons are defined in the application's header file as resources, which 
    // are numbered consecutively starting with IDS_FIRSTICON. The number of 
    // icons is defined in the header file as C_ICONS.
    /*
        for(index = 0; index < C_ICONS; index++)
        {
            hIconItem = LoadIcon (g_hinst, MAKEINTRESOURCE(IDS_FIRSTICON + index));
            ImageList_AddIcon(hSmall, hIconItem);
            ImageList_AddIcon(hLarge, hIconItem);
            Destroy(hIconItem);
        }
    */

    // Assign the image lists to the list-view control. 
    ListView_SetImageList(m_hWnd, hLarge, LVSIL_NORMAL);
    ListView_SetImageList(m_hWnd, hSmall, LVSIL_SMALL);

    return TRUE;
}

void wwt::ListView::ResizeView(int main_col_index)
{
    RECT rect;
    int nOffset = 0;

    for (size_t i = 0; i < this->m_vColumns.size(); ++i)
    {
        if (i == main_col_index)
            continue;
        nOffset += this->m_vColumns[i].size;
    }

    GetClientRect(this->m_hWnd, &rect);
    this->m_vColumns[main_col_index].size = rect.right - nOffset;
    ListView_SetColumnWidth(m_hWnd, 0, this->m_vColumns[main_col_index].size);

    HWND parent = GetParent(m_hWnd);
    GetClientRect(parent, &rect);
    MoveWindow(m_hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
}
