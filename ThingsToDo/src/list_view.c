#include "list_view.h"

#define ISBRIGHT(c) ((c & ~(0xFFFFFF << 8)) > 0x8F) || ((c & ~(0xFFFF << 16)) > 0x8FFF)
#define ROW_MAX_DEFAULT 100
#define ELEMENT_TEXT_MAX 256

BOOL ListViewInit(ListView* lw, const int id, const HWND hParent)
{
    lw->iID = id;
    lw->cCapacity = ROW_MAX_DEFAULT;
    lw->cRows = 0;
    lw->cPending = 0;
    lw->dwSelected = -1;
    lw->hWnd = GetDlgItem(hParent, id);
    lw->lwRows = (ListViewRow*)malloc(sizeof(ListViewRow) * ROW_MAX_DEFAULT);

#ifdef UNICODE
    ListView_SetUnicodeFormat(lw->hWnd, TRUE);
#endif

    //SetWindowLongPtr(lw->hWnd, GWLP_USERDATA, lw);
    return lw->hWnd != INVALID_HANDLE_VALUE;
}

void ListViewDestroy(ListView* lw)
{
    for (DWORD i = 0; i < lw->cPending; i++)
    {
        ListViewFreeRow(lw, i);
    }

    free(lw->lwRows);
}

void ListViewSetStyle(ListView* lw, const DWORD dwStyle)
{
    lw->dwStyle = dwStyle;
    ListView_SetExtendedListViewStyle(lw->hWnd, dwStyle);
}

void ListViewSetColumns(ListView* lw, const DWORD cColumns, ListViewCol pColumns[])
{
    lw->lwCols = pColumns;
    lw->cCols = cColumns;

    for (DWORD dwIndex = 0; dwIndex < cColumns; dwIndex++) {
        LVCOLUMN lvc;

        ZeroStruct(lvc);

        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvc.cx = pColumns[dwIndex].dwWidth;
        lvc.pszText = (LPTSTR)pColumns[dwIndex].lpszName;

        ListView_InsertColumn(lw->hWnd, dwIndex, &lvc);
    }
}

ListViewRow* ListViewCreateRow(ListView* lw, DWORD dwColor, BOOL bChecked, LPCTSTR* rgText)
{
    ListViewRow* newRow = (ListViewRow*)malloc(sizeof(ListViewRow));
    Try(!newRow, TEXT("Failed to allocate memory for new row"));
    newRow->bChecked = bChecked;
    newRow->dwColor = dwColor;
    newRow->rgText = (ListViewText*)malloc(sizeof(ListViewText) * lw->cCols);
    Try(!newRow->rgText, TEXT("Failed to allocate memory for row data"));

    for (BYTE i = 0; i < lw->cCols; i++)
    {
        newRow->rgText[i].dwSize = (DWORD)_tcslen(rgText[i]);
        size_t newSize = (newRow->rgText[i].dwSize + 1) * sizeof(TCHAR);
        
        newRow->rgText[i].lpszText = (LPCTSTR)malloc(newSize);

        memcpy_s(newRow->rgText[i].lpszText, newSize, rgText[i], newSize);
    }

    return newRow;
}

void ListViewDeleteRow(ListView* lw, int iIndex)
{
    ListViewFreeRow(lw, iIndex);
    ListViewDefragment(lw, iIndex);
    ListView_DeleteItem(lw->hWnd, iIndex);
    lw->cPending--; lw->cRows--;
}

void ListViewPushRow(ListView* lw, ListViewRow* lwRow)
{
    lw->lwRows[lw->cPending++] = *lwRow;
}

void ListViewInsertRow(ListView* lw, ListViewRow* lwRow, int iIndex)
{
    Try(iIndex < 0, TEXT("Invalid item index."));
    lw->lwRows[iIndex] = *lwRow;
}

void ListViewReplaceRow(ListView* lw, int iIndex, LPCTSTR* rgText)
{
    ListViewRow* changedRow = ListViewCreateRow(
        lw,
        lw->lwRows[iIndex].dwColor,
        lw->lwRows[iIndex].bChecked,
        rgText
    );

    ListViewFreeRow(lw, iIndex);
    ListViewInsertRow(lw, changedRow, iIndex);
}

void ListViewFreeRow(ListView* lw, int iIndex)
{
    Try(iIndex < 0, TEXT("Invalid item index."));

    for (BYTE i = 0; i < lw->cCols; i++)
    {
        free(lw->lwRows[iIndex].rgText[i].lpszText);
    }

    free(lw->lwRows[iIndex].rgText);
}

void ListViewDefragment(ListView* lw, int iStart)
{
    if (iStart >= lw->cPending) return;
    size_t size = (lw->cPending - iStart) * sizeof(ListViewRow);
    memmove_s(
        &lw->lwRows[iStart],
        size,
        &lw->lwRows[iStart + 1],
        size);
}

void ListViewUpdate(ListView* lw)
{
    LVITEM lvI;
    ZeroStruct(lvI);

    lvI.pszText     = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
    lvI.cchTextMax  = ELEMENT_TEXT_MAX;
    lvI.mask        = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
    lvI.state       = 0;
    lvI.iSubItem    = 0;

    /* vertical (rows) */
    for (int index = lw->cRows; index < lw->cPending; ++index)
    {
        lvI.iItem = index;
        lvI.iImage = index;

        // Insert items into the list.
        if (ListView_InsertItem(lw->hWnd, &lvI) == -1)
            return FALSE;

        // Set checkbox checked
        ListView_SetCheckState(lw->hWnd, index, lw->lwRows[index].bChecked);
    }

    lw->cRows = lw->cPending;

    return TRUE;
}

LRESULT ListViewProcessCustomDraw(ListView* lw, LPARAM lParam)
{
    LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;

    switch (lplvcd->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT: //Before the paint cycle begins
        //request notifications for individual listview items
        return CDRF_NOTIFYITEMDRAW;

    case CDDS_ITEMPREPAINT: //Before an item is drawn
    {
        DWORD color = (lw->lwRows[lplvcd->nmcd.dwItemSpec].dwColor) & ~(0xFF << 24);

        lplvcd->clrText = ISBRIGHT(color) ? RGB(0, 0, 0) : RGB(255, 255, 255);
        lplvcd->clrTextBk = color;

        return CDRF_NEWFONT;
    }
    break;
    }
    return CDRF_DODEFAULT;
}
