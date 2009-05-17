// SpellingOptions.cpp : implementation file
//

#include "SpellingOptionsDlg.h"
#include "LanguageDlg.h"
#include <windows.h>

extern HINSTANCE g_hInstDll;

// CSpellingOptionsDlg dialog

static CSpellingOptionsDlg* s_pThis;

CSpellingOptionsDlg::CSpellingOptionsDlg(TolonSpellCheck::CSession* pSession, HWND hwndParent /*=NULL*/) :
	m_pSession(pSession),
	m_hwnd(NULL),
	m_hwndParent(hwndParent)
{

}

CSpellingOptionsDlg::~CSpellingOptionsDlg()
{
}

int CSpellingOptionsDlg::DoModal()
{
	s_pThis = this;
	DialogBox(g_hInstDll, MAKEINTRESOURCE(CSpellingOptionsDlg::IDD), m_hwndParent, CSpellingOptionsDlg::WndProc);
	return 0;
}

// CSpellingOptionsDlg message handlers
BOOL CSpellingOptionsDlg::OnInitDialog()
{    
	return TRUE;
}

void CSpellingOptionsDlg::OnDicLangClicked()
{
	CLanguageDlg dlg(m_pSession, m_hwnd);
	dlg.DoModal();
}

int CALLBACK CSpellingOptionsDlg::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!s_pThis)
		return 0;
	
	if (!s_pThis->m_hwnd)
		s_pThis->m_hwnd = hDlg;
	
	switch (message)
	{
	case WM_INITDIALOG:
		return s_pThis->OnInitDialog();

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON1)
		{
			s_pThis->OnDicLangClicked();
		}
		break;
	}
	return (INT_PTR)FALSE;
}