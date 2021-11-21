#include "list_view.h"
#include "dialogs.h"

#define TEXT_BUFFER_SIZE 256

static HWND hEditLabel = INVALID_HANDLE_VALUE;

BOOL ListViewOnNotify(ListView* lw, LPARAM lParam)
{
    NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;

    switch (((LPNMHDR)lParam)->code)
    {

    case LVN_GETDISPINFO:

        // sets text to a ListView field
        plvdi->item.pszText = (LPTSTR)lw->lwRows[plvdi->item.iItem].rgText[plvdi->item.iSubItem].lpszText;
        break;

    case LVN_BEGINLABELEDIT:

        hEditLabel = ListView_GetEditControl(lw->hWnd); // probably unused
        break;

    case LVN_ENDLABELEDIT:
    {
        if (plvdi->item.pszText)
        {
            TCHAR timeBuffer[32];
            GetTime(timeBuffer);

            plvdi->item.iSubItem = 0;     // we get the item only (change for sub)

            ListViewReplaceRow(lw, lw->dwSelected, (LPCTSTR[2]) {
                plvdi->item.pszText,
                timeBuffer
            });

            plvdi->item.pszText = lw->lwRows[lw->dwSelected].rgText[0].lpszText;
        }

        break;
    }

    case LVN_ITEMCHANGED:
    {
        LPNMLISTVIEW pNMLV = (LPNMLISTVIEW)lParam;
        lw->dwSelected = ListView_GetNextItem(lw->hWnd, -1, LVNI_FOCUSED);

        if (pNMLV->uChanged & LVIF_STATE && pNMLV->uOldState)
        {
            switch (pNMLV->uNewState & LVIS_STATEIMAGEMASK)
            {
            case 0x2000: /*checked*/
                lw->lwRows[pNMLV->iItem].bChecked = TRUE;
                break;
            case 0x1000: /*unchecked*/
                lw->lwRows[pNMLV->iItem].bChecked = FALSE;
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
            lw->dwSelected = ListView_GetNextItem(lw->hWnd, -1, LVNI_SELECTED);

            if (lw->dwSelected == -1)
                return;

            ListViewDeleteRow(lw, lw->dwSelected);
        }
        break;
    }

    case NM_DBLCLK:
        DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_VIEW_ENTRY), lw->hWnd, ViewEntry, lw);
        break;

    case NM_RETURN:

        ListView_EditLabel(lw->hWnd, lw->dwSelected);
        break;
    }

    return 0;
}
