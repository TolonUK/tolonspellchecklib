#pragma once

#include "resource.h"
#include "TolonSpellCheckInternals.h"
#include "tscSession.h" // for CSessionOptions

// CSpellingOptionsDlg dialog

class CSpellingOptionsDlg
{
protected:
	static int CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
    CSpellingOptionsDlg(TolonSpellCheck::CSessionOptions& options, HWND hwndParent = NULL);   // standard constructor
	~CSpellingOptionsDlg();

// Dialog Data
	enum { IDD = IDD_SPELLING_OPTIONS };
	
	int DoModal();

    HWND GetHwnd() const { return m_hwnd; }

protected:
	BOOL OnInitDialog();

public:
	void OnDicLangClicked();
    void OnOk();

protected:
    TolonSpellCheck::CSessionOptions& m_options;
	TolonSpellCheck::CSession* const m_pSession; // to be removed
	HWND m_hwnd;
	HWND m_hwndParent;
};
