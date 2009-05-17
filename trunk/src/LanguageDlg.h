#pragma once

#include "resource.h"
#include "TolonSpellCheckInternals.h"

// CLanguageDlg dialog

class CLanguageDlg 
{
protected:
	static int CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	static bool LangEnumCallback(TolonSpellCheck::LANGUAGE_DESC_WIDEDATA* pData, void* pUserData);

public:
	CLanguageDlg(TolonSpellCheck::CSession* pSession, HWND hwndParent = NULL);   // standard constructor
	~CLanguageDlg();

// Dialog Data
	enum { IDD = IDD_DIC_LANG };
	
	int DoModal();

protected:
	BOOL OnInitDialog();
	void InitLangList();

protected:
	TolonSpellCheck::CSession* const m_pSession;
	HWND m_hwnd;
	HWND m_hwndParent;
	HWND m_hwndLangList;
};
