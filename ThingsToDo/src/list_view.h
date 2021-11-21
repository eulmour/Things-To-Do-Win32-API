#pragma once
#include "things_to_do.h"

#define LM_SELECTALL (WM_USER + 0x0300)
#define LM_DELETE (WM_USER + 0x0301)

typedef struct ListViewText
{
	DWORD		dwSize;
	LPCTSTR		lpszText;
} ListViewText;

typedef struct ListViewCol
{
	LPCTSTR		lpszName;
	DWORD		dwWidth;

} ListViewCol;

typedef struct ListViewRow
{
	BOOL			bChecked;
	DWORD			dwColor;
	ListViewText*	rgText;

} ListViewRow;

typedef struct ListView
{
	int				iID;
	HWND			hWnd;
	DWORD			dwStyle;
	DWORD			dwSelected;
	ListViewCol*	lwCols;
	ListViewRow*	lwRows;
	DWORD			cCols;
	DWORD			cRows;
	DWORD			cPending;
	DWORD			cCapacity;

} ListView;

BOOL			ListViewInit				(ListView* lw, const int id, const HWND hParent);
void			ListViewDestroy				(ListView* lw);
void			ListViewSetStyle			(ListView* lw, const DWORD dwStyle);
void			ListViewSetColumns			(ListView* lw, const DWORD cColumns, ListViewCol pColumns[]);
BOOL			ListViewOnNotify			(ListView* lw, LPARAM lParam);
ListViewRow*	ListViewCreateRow			(ListView* lw, DWORD dwColor, BOOL bChecked, LPCTSTR* rgText);
void			ListViewDeleteRow			(ListView* lw, int iIndex);
void			ListViewPushRow				(ListView* lw, ListViewRow* lwRow);
void			ListViewInsertRow			(ListView* lw, ListViewRow* lwRow, int iIndex);
void			ListViewReplaceRow			(ListView* lw, int iIndex, LPCTSTR* rgText);
void			ListViewFreeRow				(ListView* lw, int iIndex);
void			ListViewDefragment			(ListView* lw, int iStart);
void			ListViewUpdate				(ListView* lw);
LRESULT			ListViewProcessCustomDraw	(ListView* lw, LPARAM lParam);