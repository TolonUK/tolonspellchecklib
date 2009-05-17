#pragma once

#include "resource.h"
#include "TolonSpellCheckInternals.h"

// CSpellingOptionsDlg dialog

class CSpellingOptionsDlg
{
protected:
	static int CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
	CSpellingOptionsDlg(TolonSpellCheck::CSession* pSession, HWND hwndParent = NULL);   // standard constructor
	~CSpellingOptionsDlg();

// Dialog Data
	enum { IDD = IDD_SPELLING_OPTIONS };
	
	int DoModal();

protected:
	BOOL OnInitDialog();

public:
	void OnDicLangClicked();

protected:
	TolonSpellCheck::CSession* const m_pSession;
	HWND m_hwnd;
	HWND m_hwndParent;
};
