// SpellingOptions.cpp : implementation file
//

#include "SpellingOptionsDlg.h"
#include "LanguageDlg.h"
#include "tscSession.h"
#include <windows.h>
#include "WndUtils.h"

extern HINSTANCE g_hInstDll;

// CSpellingOptionsDlg dialog

static CSpellingOptionsDlg* s_pThis;

CSpellingOptionsDlg::CSpellingOptionsDlg(TolonSpellCheck::CSessionOptions& options, HWND hwndParent /*=NULL*/) :
    m_options(options),
	m_pSession(NULL), // to be removed
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
	return DialogBox(g_hInstDll, MAKEINTRESOURCE(CSpellingOptionsDlg::IDD), m_hwndParent, CSpellingOptionsDlg::WndProc);
}

// CSpellingOptionsDlg message handlers
BOOL CSpellingOptionsDlg::OnInitDialog()
{
    CWndUtils::CheckDlgButton(GetHwnd(), IDC_MAIN_DIC_ONLY, m_options.bIgnoreUserDictionaries);
    CWndUtils::CheckDlgButton(GetHwnd(), IDC_IGNORE_UPPERCASE, m_options.bIgnoreUppercaseWords);
    CWndUtils::CheckDlgButton(GetHwnd(), IDC_IGNORE_WORDS_WITH_NUMBERS, m_options.bIgnoreWordsWithNumbers);
    CWndUtils::CheckDlgButton(GetHwnd(), IDC_IGNORE_URIS, m_options.bIgnoreUris);

	return TRUE;
}

void CSpellingOptionsDlg::OnDicLangClicked()
{
    TolonSpellCheck::CSessionOptions options_copy(m_options);

	CLanguageDlg dlg(options_copy, m_hwnd);
	if (dlg.DoModal() == IDOK)
    {
        m_options = options_copy;
    }
}

void CSpellingOptionsDlg::OnOk()
{
    m_options.bIgnoreUserDictionaries = CWndUtils::IsDlgButtonChecked(GetHwnd(), IDC_MAIN_DIC_ONLY);
    m_options.bIgnoreUppercaseWords = CWndUtils::IsDlgButtonChecked(GetHwnd(), IDC_IGNORE_UPPERCASE);
    m_options.bIgnoreWordsWithNumbers = CWndUtils::IsDlgButtonChecked(GetHwnd(), IDC_IGNORE_WORDS_WITH_NUMBERS);
    m_options.bIgnoreUris = CWndUtils::IsDlgButtonChecked(GetHwnd(), IDC_IGNORE_URIS);
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
            s_pThis->OnOk();
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
