#pragma once

#include "resource.h"
#include "TolonSpellCheckInternals.h"

// CCheckSpellingDlg dialog

class CCheckSpellingDlg
{
protected:
	static int CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CCheckSpellingDlg(HWND hwndParent = NULL);   // standard constructor
	~CCheckSpellingDlg();

// Dialog Data
	enum { IDD = IDD_CHECK_SPELLING };
	
	int DoModal();

protected:
	BOOL OnInitDialog();

protected:
	HWND m_hwnd;
	HWND m_hwndParent;
};
