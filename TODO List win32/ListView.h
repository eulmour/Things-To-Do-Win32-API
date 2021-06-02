#pragma once
#include "BaseClass.h"
#include <Windows.h>
#include <commctrl.h>

#include <vector>
#include <string>
#include <initializer_list>

#define BUFFER_SIZE 100

namespace wwt
{
	struct RndItem
	{
		int   iIcon;                 // Bitmap assigned to this item.
		UINT  state;                 // Item state value.
		TCHAR Title[BUFFER_SIZE];    // BUFFER_SIZE is a user-defined macro value.
		TCHAR SubText1[BUFFER_SIZE]; // Text for the label of the first sub-item.
		TCHAR SubText2[BUFFER_SIZE]; // Text for the label of the second item.
	};

	struct Column
	{
		std::wstring caption;
		int size;
		std::vector<std::pair<std::wstring, int>> items;
	};

	class ListView : public BaseClass<ListView>
	{
	public:
		ListView() = delete;
		ListView(std::initializer_list<Column> cols);

		PCWSTR ClassName() const override { return WC_LISTVIEW; };		

		BOOL Initialize();
		BOOL UpdateList();
		BOOL UpdateCheckBoxState();
		void Clear();
		BOOL OnNotify(LPARAM lParam);
		void UpdateMenu(HMENU hMenu);
		void SetView(DWORD dwView);
		void SetStyle(DWORD dwView);
		void HighlightItems(int index = -1) const;
		void PopItem();

		BOOL InitListViewImageLists(int resource_id);
		void InsertItem(int col, LPCWSTR caption) { m_vColumns[col].items.push_back(std::make_pair(std::wstring(caption), 0)); };
		void ResizeView(int main_col_index);

		size_t GetColCount() const { return m_vColumns.size(); }
		size_t GetRowCount() const { return this->m_vColumns[0].items.size(); }

		std::vector<Column>& RetrieveData() { return m_vColumns; }
		HWND& RetrieveHwnd() { return m_hWnd; }
	protected:
		HWND m_hEdit = nullptr, m_hEditLabel = nullptr;
		size_t m_nRows = 0;
		int iSelected = -1;
		bool m_bPreventEdit = false;
		std::vector<Column> m_vColumns;
	};
}

