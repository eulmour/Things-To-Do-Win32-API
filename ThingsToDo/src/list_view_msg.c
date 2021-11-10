#include "list_view.h"

#define TEXT_BUFFER_SIZE 256

static enum Edit
{
    NORMAL, PREVENT
} EditMode = NORMAL;

static DWORD dwSelected = -1;
static HWND hEditLabel = INVALID_HANDLE_VALUE;

BOOL ListViewOnNotify(ListView* lw, LPARAM lParam)
{
    NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;

    switch (((LPNMHDR)lParam)->code)
    {

    case LVN_GETDISPINFO:

        // sets text to a ListView field
        plvdi->item.pszText = (LPTSTR)lw->lwRows[plvdi->item.iItem]->rgTextFields[plvdi->item.iSubItem];
        //plvdi->item.pszText = (LPTSTR)lw->lwRows[plvdi->item.iItem]->rgText[plvdi->item.iSubItem].lpszText;
        break;

    case LVN_BEGINLABELEDIT:

        hEditLabel = ListView_GetEditControl(lw->hWnd);
        break;

    case LVN_ENDLABELEDIT:
    {
        if (EditMode == NORMAL)
        {
            TCHAR timeBuffer[32];
            GetTime(timeBuffer);

            plvdi->item.iSubItem = 0;     // we get the item only (change for sub)

            ListViewRow* changedRow = ListViewCreateRow(
                lw,
                lw->lwRows[dwSelected]->dwColor,
                lw->lwRows[dwSelected]->bChecked,
                (LPCTSTR[2]) {
                    plvdi->item.pszText,
                    timeBuffer
                }
            );

            ListViewFreeRow(lw, dwSelected);
            ListViewInsertRow(lw, changedRow, dwSelected);

            plvdi->item.pszText = changedRow->rgTextFields[0];
            //plvdi->item.pszText = changedRow->rgText[0].lpszText;
        }

        EditMode = NORMAL;
        break;
    }

    case LVN_ITEMCHANGED:
    {
        LPNMLISTVIEW pNMLV = (LPNMLISTVIEW)lParam;
        dwSelected = ListView_GetNextItem(lw->hWnd, -1, LVNI_FOCUSED);

        if (pNMLV->uChanged & LVIF_STATE && pNMLV->uOldState)
        {
            switch (pNMLV->uNewState & LVIS_STATEIMAGEMASK)
            {
            case 0x2000: /*checked*/
                lw->lwRows[pNMLV->iItem]->bChecked = TRUE;
                break;
            case 0x1000: /*unchecked*/
                lw->lwRows[pNMLV->iItem]->bChecked = FALSE;
                break;
            }
        }
        break;
    }

    case LM_SELECTALL:

        for (int i = 0; i < lw->cPending; ++i)
        {
            ListView_SetItemState(lw->hWnd, i, LVIS_SELECTED, 0x00f);
        }

        break;

    case LM_DELETE:
    {
        while (TRUE)
        {
            dwSelected = ListView_GetNextItem(lw->hWnd, -1, LVNI_SELECTED);

            if (dwSelected == -1)
                return;

            ListViewDeleteRow(lw, dwSelected);
        }
        break;
    }

    case NM_DBLCLK:

        //PostMessage(GetParent(lw->hWnd), NF_MSG, IDC_LIST_ENTRIES /* Main list */, dwSelected);
        break;

    case NM_RETURN:

        ListView_EditLabel(lw->hWnd, dwSelected);
        break;
    }

    return 0;
}
