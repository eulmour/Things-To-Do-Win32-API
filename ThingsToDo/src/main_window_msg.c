#include "things_to_do.h"

#include "main_window.h"
#include "main_window_msg.h"

#include "util.h"
#include "dialogs.h"
#include "Resource.h"
#include "file.h"
#include "list_view.h"

extern ListView lw;
static LPCTSTR sCurrentFilePath;
static HANDLE hFile;

INT_PTR CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        InitCommonControls();
        InitWindowControls(hWnd);
        InitFromFile();

        SendMessage(
            hWnd,
            WM_SETICON,
            (WPARAM)1,
            (LPARAM)LoadIcon(GetModuleHandle(NULL),
                MAKEINTRESOURCE(IDI_SMALL)));

        ShowWindow(hWnd, SW_NORMAL);
        UpdateWindow(hWnd);

        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case ID_FILE_NEW:
            ListView_DeleteAllItems(lw.hWnd);
            ListViewDestroy(&lw);
            assert(

                ListViewInit(&lw, IDC_LIST_ENTRIES, hWnd) &&
                "Error during ListViewInit: Unable to initialize window."
            );

            break;

        case ID_FILE_OPEN:
        {
            sCurrentFilePath = ShowFileDialog(TEXT("Text files\0*.txt\0All files\0*.*\0"), hWnd, FALSE);
            SetTitle(sCurrentFilePath);

            hFile = HandleFile(sCurrentFilePath, FALSE);
            SendMessage(hWnd, WM_COMMAND, ID_FILE_NEW, NULL);

            if (ReadDataFromFile(&lw) == S_FALSE) MessageBox(hWnd, TEXT("Error. File not found"), TEXT("Error"), MB_OK | MB_ICONERROR);
            ListViewUpdate(&lw);

            break;
        }

        case ID_FILE_SAVE:
        {
            hFile = HandleFile(sCurrentFilePath, TRUE);

            if (WriteDataToFile(&lw) == S_FALSE) MessageBox(hWnd, TEXT("Error while saving file"), TEXT("Error"), MB_OK | MB_ICONERROR);

            break;
        }

        case ID_EDIT_CUT:
            SendListViewMessage(hWnd, IDC_LIST_ENTRIES, LM_DELETE);
            break;

        case ID_EDIT_SELECTALL:
            SendListViewMessage(hWnd, IDC_LIST_ENTRIES, LM_SELECTALL);
            break;

        case ID_EDIT_INSERTNEW:
        {
            TCHAR timeBuffer[32];
            GetTime(timeBuffer);

            ListViewRow* newRow = ListViewCreateRow(&lw, 0xFFFFFF, FALSE, (LPCTSTR[2]) {
                TEXT(""), timeBuffer
            });

            ListViewPushRow(&lw, newRow);
            ListViewUpdate(&lw);

            break;
        }
        case ID_EDIT_DELETE:
            SendListViewMessage(hWnd, IDC_LIST_ENTRIES, LM_DELETE);
            break;

        case IDM_ABOUT:
            DialogBox(app.hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return (INT_PTR)FALSE;
        }
    }
    break;

    case WM_NOTIFY:
    {
        if (lw.iID == LOWORD(wParam))
        {
            LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

            if (pnm->hdr.hwndFrom == lw.hWnd && pnm->hdr.code == NM_CUSTOMDRAW)
            {
                SetWindowLongPtr(hWnd, 0, (LONG)ListViewProcessCustomDraw(&lw, lParam));
                break;
            }

            ListViewOnNotify(&lw, lParam);
        }

        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_SIZE:
        Resize(hWnd);

        if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
            Resize(hWnd);

        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        ListViewDestroy(&lw);
        PostQuitMessage(0);
        break;

    default:
        return (INT_PTR)FALSE;
    }
    return (INT_PTR)TRUE;
}
