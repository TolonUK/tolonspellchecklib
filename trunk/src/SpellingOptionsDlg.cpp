// SpellingOptions.cpp : implementation file
//

#include "SpellingOptionsDlg.h"
#include "LanguageDlg.h"
#include "tscSession.h"
#include <windows.h>
#include "WndUtils.h"
#include <sstream>

extern HINSTANCE g_hInstDll;

using namespace TolonUI::Windows;

// CSpellingOptionsDlg dialog

static CSpellingOptionsDlg* s_pThis;

CSpellingOptionsDlg::CSpellingOptionsDlg(TolonSpellCheck::CSessionOptionsData& options, HWND hwndParent /*=NULL*/) :
    m_options(options),
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
    CWndUtils::CheckDlgButton(GetHwnd(), IDC_MAIN_DIC_ONLY, m_options.IgnoreUserDictionaries());
    CWndUtils::CheckDlgButton(GetHwnd(), IDC_IGNORE_UPPERCASE, m_options.IgnoreUppercaseWords());
    CWndUtils::CheckDlgButton(GetHwnd(), IDC_IGNORE_WORDS_WITH_NUMBERS, m_options.IgnoreWordsWithNumbers());
    CWndUtils::CheckDlgButton(GetHwnd(), IDC_IGNORE_URIS, m_options.IgnoreUris());

    return TRUE;
}

void CSpellingOptionsDlg::OnDicLangClicked()
{
    TolonSpellCheck::CSessionOptionsData options_copy(m_options);

    CLanguageDlg dlg(options_copy, m_hwnd);
    const int nResult = dlg.DoModal();
    if (nResult == IDOK)
    {
#ifdef DEBUG
        {
            std::wstringstream ss;
            ss << L"CSpellingOptionsDlg::OnDicLangClicked is saving new options." << std::endl;
            ss << L"Old options: " << m_options << std::endl;
            ss << L"New options: " << options_copy << std::endl;
            OutputDebugString(ss.str().c_str());
        }
#endif

        m_options = options_copy;
    }
    else
    {
        std::wstringstream ss;
        ss << L"CSpellingOptionsDlg::OnDicLangClicked, dlg.DoModal() returned " << nResult;
        OutputDebugString(ss.str().c_str());
    }
}

void CSpellingOptionsDlg::OnOk()
{
    m_options.IgnoreUserDictionaries(CWndUtils::IsDlgButtonChecked(GetHwnd(), IDC_MAIN_DIC_ONLY));
    m_options.IgnoreUppercaseWords(CWndUtils::IsDlgButtonChecked(GetHwnd(), IDC_IGNORE_UPPERCASE));
    m_options.IgnoreWordsWithNumbers(CWndUtils::IsDlgButtonChecked(GetHwnd(), IDC_IGNORE_WORDS_WITH_NUMBERS));
    m_options.IgnoreUris(CWndUtils::IsDlgButtonChecked(GetHwnd(), IDC_IGNORE_URIS));
}

int CALLBACK CSpellingOptionsDlg::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
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
        else if (LOWORD(wParam) == IDC_DICLANG_BTN)
        {
            s_pThis->OnDicLangClicked();
        }
        break;
    }
    return (INT_PTR)FALSE;
}
