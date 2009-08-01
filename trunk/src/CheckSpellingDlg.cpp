#include "CheckSpellingDlg.h"
#include "tscSession.h"
#include <windows.h>
#include <assert.h>
#include <sstream>
#include <string>
#include <vector>

// CCheckSpellingDlg dialog

extern HINSTANCE g_hInstDll;

using namespace TolonSpellCheck;

static CCheckSpellingDlg* s_pThis;

const UINT_PTR CCheckSpellingDlg::m_nCheckerPollEvent = 1;
static const UINT s_nCheckerPollFrequency = 250; // milliseconds

CCheckSpellingDlg::CCheckSpellingDlg(TolonSpellCheck::CSession* pSession, TSC_CHECKSPELLING_DATA* pData) :
	m_pSession(pSession),
    m_pData(pData),
    m_hwnd(NULL),
    m_hwndParent(NULL),
    m_checker(pSession),
    m_nLastCheckerState(CRichEditSpellChecker::SpellCheckState_INVALID),
    m_dwLastCheckerCharsDone(0),
    m_dwLastCheckerCharsTotal(0)
{
    if (pData)
    {
        m_hwndParent = pData->hwndParent;
    }
}

CCheckSpellingDlg::~CCheckSpellingDlg()
{
}

int CCheckSpellingDlg::DoModal()
{
	s_pThis = this;
	DialogBox(g_hInstDll, MAKEINTRESOURCE(CCheckSpellingDlg::IDD), m_hwndParent, &CCheckSpellingDlg::WndProc);
	return 0;
}

BOOL CCheckSpellingDlg::OnInitDialog()
{
    TolonSpellCheck::CSession* pSession = GetSession();

    if (!pSession)
        return FALSE;

    tsc_result r = 0;

    // Set the title bar text to indicate the current language.
    std::vector<wchar_t> sLangCode(13);
    LANGUAGE_DESC_WIDEDATA LangDesc = {0};
    LangDesc.cbSize = sizeof(LANGUAGE_DESC_WIDEDATA);

    r = pSession->GetCurrentLanguage(&(*sLangCode.begin()));
    if (TSC_SUCCEEDED(r))
        r = pSession->DescribeLanguage(&(*sLangCode.begin()), &LangDesc);

    if (TSC_SUCCEEDED(r))
    {
        std::wstringstream ss;
        ss << L"Spelling: " << LangDesc.wszDisplayName;
        ::SetWindowText(GetHwnd(), ss.str().c_str());
    }

    // Start the spell check polling timer;
    ::SetTimer(GetHwnd(), m_nCheckerPollEvent, s_nCheckerPollFrequency, &TimerProc);

    // Begin the spell check!
    m_checker.StartSpellCheck(GetRichEditHwnd());

	return TRUE;
}

int CALLBACK CCheckSpellingDlg::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    BOOL bHandled = FALSE;

	if (!s_pThis)
		return 0;
	
	if (!s_pThis->m_hwnd)
		s_pThis->m_hwnd = hDlg;
	
	switch (message)
	{
	case WM_INITDIALOG:
		return s_pThis->OnInitDialog();

	case WM_COMMAND:
        const WORD nCmd = LOWORD(wParam);
        
		if (nCmd == IDOK || nCmd == IDCANCEL)
		{ EndDialog(hDlg, LOWORD(wParam)); }
        else if (nCmd == IDC_OPTIONS)
            { s_pThis->OnCmdOptions(); }
        else if (nCmd == IDC_CANCEL_SPELLCHECK)
            { s_pThis->OnCmdCancelSpellCheck(); }
        else if (nCmd == IDC_IGNORE_ONCE)
            { s_pThis->OnCmdIgnoreOnce(); }
        else if (nCmd == IDC_IGNORE_ALL)
            { s_pThis->OnCmdIgnoreAll(); }
        else if (nCmd == IDC_ADD_TO_DICTIONARY)
            { s_pThis->OnCmdAddToDictionary(); }
        else if (nCmd == IDC_CHANGE)
            { s_pThis->OnCmdChange(); }
        else if (nCmd == IDC_CHANGE_ALL)
            { s_pThis->OnCmdChangeAll(); }

        bHandled = TRUE;
		break;
	}

	return static_cast<INT_PTR>(bHandled);
}

void CCheckSpellingDlg::OnCmdOptions()
{
    TolonSpellCheck::CSession* pSession = GetSession();

    if (pSession)
    {
        tsc_result r = 0;

        TSC_SHOWOPTIONSWINDOW_DATA data = {0};
        data.cbSize = sizeof(TSC_SHOWOPTIONSWINDOW_DATA);
        data.hWndParent = GetHwnd();

        r = pSession->ShowOptionsWindow(&data);
        if (FAILED(r))
        {
            ::MessageBox(GetHwnd(), L"Failed to display options window.", L"TolonSpellCheck", MB_OK | MB_ICONEXCLAMATION);
        }
    }
}

void CCheckSpellingDlg::OnCmdCancelSpellCheck()
{
    ::MessageBox(GetHwnd(), L"OnCmdCancelSpellCheck()", L"TolonSpellCheck", MB_OK | MB_ICONINFORMATION);
}

void CCheckSpellingDlg::OnCmdIgnoreOnce()
{
    ::MessageBox(GetHwnd(), L"OnCmdIgnoreOnce()", L"TolonSpellCheck", MB_OK | MB_ICONINFORMATION);
}

void CCheckSpellingDlg::OnCmdIgnoreAll()
{
    ::MessageBox(GetHwnd(), L"OnCmdIgnoreAll()", L"TolonSpellCheck", MB_OK | MB_ICONINFORMATION);
}

void CCheckSpellingDlg::OnCmdAddToDictionary()
{
    ::MessageBox(GetHwnd(), L"OnCmdAddToDictionary()", L"TolonSpellCheck", MB_OK | MB_ICONINFORMATION);
}

void CCheckSpellingDlg::OnCmdChange()
{
    ::MessageBox(GetHwnd(), L"OnCmdChange()", L"TolonSpellCheck", MB_OK | MB_ICONINFORMATION);
}

void CCheckSpellingDlg::OnCmdChangeAll()
{
    ::MessageBox(GetHwnd(), L"OnCmdChangeAll()", L"TolonSpellCheck", MB_OK | MB_ICONINFORMATION);
}

HWND CCheckSpellingDlg::GetRichEditHwnd() const
{
    HWND hwnd = NULL;
    if (m_pData)
    {
        hwnd = m_pData->hwnd;
    }
    return hwnd;
}

void CCheckSpellingDlg::TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
    if (!s_pThis)
        return;

    if (hwnd != s_pThis->GetHwnd())
        return;

    if (idEvent == m_nCheckerPollEvent)
        s_pThis->OnTimer_PollChecker();
}

void CCheckSpellingDlg::OnTimer_PollChecker()
{
    std::wstringstream ss;
    ss << ::GetTickCount() << L" OnTimer_PollChecker()" << std::endl;
    ::OutputDebugString(ss.str().c_str());

    CRichEditSpellChecker::State nState = CRichEditSpellChecker::SpellCheckState_INVALID;
    DWORD dwCharsDone = 0;
    DWORD dwCharsTotal = 0;

    m_checker.GetState(nState, dwCharsDone, dwCharsTotal);

    if (    (nState != m_nLastCheckerState) 
         || (dwCharsDone != m_dwLastCheckerCharsDone)
         || (dwCharsTotal != m_dwLastCheckerCharsTotal) )
    {
        m_nLastCheckerState = nState;
        m_dwLastCheckerCharsDone = dwCharsDone;
        m_dwLastCheckerCharsTotal = dwCharsTotal;

        UpdateUI();
    }
}

void CCheckSpellingDlg::UpdateUI()
{
    BOOL bAllowSpellCheckCancel = FALSE;
    BOOL bAllowGeneralInput = FALSE;

    bAllowSpellCheckCancel = (    (m_nLastCheckerState == CRichEditSpellChecker::SpellCheckState_WORKING)
                               || (m_nLastCheckerState == CRichEditSpellChecker::SpellCheckState_WAITING) );
    bAllowGeneralInput = (m_nLastCheckerState == CRichEditSpellChecker::SpellCheckState_WAITING);

    // Do status text
    std::wstringstream ss;
    ss << m_dwLastCheckerCharsDone << L" of " << m_dwLastCheckerCharsTotal << L" characters checked." << std::endl;
    switch (m_nLastCheckerState)
    {
    case CRichEditSpellChecker::SpellCheckState_INVALID:
        ss << L"Checking not yet started.";
        break;
    case CRichEditSpellChecker::SpellCheckState_WORKING:
        ss << L"Working...";
        break;
    case CRichEditSpellChecker::SpellCheckState_WAITING:
        ss << L"Waiting for user input...";
        break;
    case CRichEditSpellChecker::SpellCheckState_COMPLETE:
        ss << L"Checking complete.";
        break;
    case CRichEditSpellChecker::SpellCheckState_CANCELLED:
        ss << L"Checking cancelled by user.";
        break;
    case CRichEditSpellChecker::SpellCheckState_ERROR:
        ss << L"An error occurred.";
        break;
    default:
        ss << L"Current state unknown!";
    }
    ::SetDlgItemText(GetHwnd(), IDC_PROGRESS_STATIC, ss.str().c_str());

    // Cancel button
    ::SendDlgItemMessage(GetHwnd(), IDC_CANCEL_SPELLCHECK, WM_ENABLE, bAllowSpellCheckCancel, NULL);

    // General user input controls
    ::SendDlgItemMessage(GetHwnd(), IDC_IGNORE_ONCE, WM_ENABLE, bAllowGeneralInput, NULL);
    ::SendDlgItemMessage(GetHwnd(), IDC_IGNORE_ALL, WM_ENABLE, bAllowGeneralInput, NULL);
    ::SendDlgItemMessage(GetHwnd(), IDC_ADD_TO_DICTIONARY, WM_ENABLE, bAllowGeneralInput, NULL);
    ::SendDlgItemMessage(GetHwnd(), IDC_CHANGE, WM_ENABLE, bAllowGeneralInput, NULL);
    ::SendDlgItemMessage(GetHwnd(), IDC_CHANGE_ALL, WM_ENABLE, bAllowGeneralInput, NULL);
    ::SendDlgItemMessage(GetHwnd(), IDC_SUGGESTION_LIST, WM_ENABLE, bAllowGeneralInput, NULL);
    ::SendDlgItemMessage(GetHwnd(), IDC_RICHEDIT, WM_ENABLE, bAllowGeneralInput, NULL);
}
