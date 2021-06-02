#pragma once
#include "BaseWindow.h"
#include "ListView.h"

#include <memory>

class MainWindow : public BaseWindow<MainWindow>
{
public:
	MainWindow();

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	PCWSTR ClassName() const { return L"Things To Do"; }
	void ShowFileDialog(BOOL bSave = FALSE);

	RECT GetClientAreaSize() const;

	friend INT_PTR CALLBACK About(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	int m_nScreenWidth, m_nScreenHeight;
	POINT cursorPoint = { 0, 0 };

	wwt::ListView m_ListView;
	std::unique_ptr<WCHAR[]> m_wCurrentFilePath;

	NMHDR m_hNotification;
	SYSTEMTIME st = { 0 };
	DWORD m_dwLastError = 0UL;
	HANDLE m_hFile = nullptr;
private:
	HRESULT _WriteComplexToFile();
	HRESULT _ReadComplexFromFile();
	void SetTitle(LPCWSTR lpText);
};
