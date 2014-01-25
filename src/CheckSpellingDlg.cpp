#include "CheckSpellingDlg.h"
#include "isoLang.h"
#include "tscSession.h"
#include "tscModule.h"
#include "WndUtils.h"
#include <windows.h>
#include <assert.h>
#include <sstream>
#include <string>
#include <vector>

// CCheckSpellingDlg dialog

extern HINSTANCE g_hInstDll;

using namespace TolonUI::Windows;

using namespace TolonSpellCheck;

static CCheckSpellingDlg* s_pThis;

const UINT_PTR CCheckSpellingDlg::m_nCheckerPollEvent = 1;
static const UINT s_nCheckerPollFrequency = 100; // milliseconds

CCheckSpellingDlg::CCheckSpellingDlg(TolonSpellCheck::CSession* pSession, TSC_CHECKSPELLING_DATA* pData) :
    m_pSession(pSession),
    m_pData(pData),
    m_hwnd(NULL),
    m_hwndParent(NULL),
    m_checker(pSession),
    m_nLastCheckerState(CRichEditSpellChecker::SpellCheckState_INVALID),
    m_dwLastCheckerCharsDone(0),
    m_dwLastCheckerCharsTotal(0),
    m_bWaitingForInput(false)
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
    return DialogBox(g_hInstDll, MAKEINTRESOURCE(CCheckSpellingDlg::IDD), m_hwndParent, &CCheckSpellingDlg::WndProc);
}

BOOL CCheckSpellingDlg::OnInitDialog()
{
    TolonSpellCheck::CSession* pSession = GetSession();

    if (!pSession)
        return FALSE;

    UpdateTitleBar();

    // Start the spell check polling timer;
    ::SetTimer(GetHwnd(), m_nCheckerPollEvent, s_nCheckerPollFrequency, &TimerProc);

    // Begin the spell check!
    m_checker.StartSpellCheck(GetRichEditHwnd());

    return TRUE;
}

void CCheckSpellingDlg::UpdateTitleBar()
{
    TolonSpellCheck::CModule* pModule = CModule::GetInstance();
    TolonSpellCheck::CSession* pSession = GetSession();

    if (pModule && pSession)
    {
        CSessionOptionsData options;
        pSession->GetSessionOptions(options);
        const wchar_t* sLang = options.CurrentLanguage();
        
        assert(sLang);
        if (sLang)
        {
            std::wstring sDisplay(sLang);
            CIsoLang::GetDisplayName(sLang, sDisplay);

            CWndUtils::SetWindowText( GetHwnd(), sDisplay.c_str(), L"Spelling: " );
        }
    }
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
        const WORD nCtlMsg = HIWORD(wParam);

        switch(nCtlMsg)
        {
        case 0:
            {
                if (nCmd == IDOK || nCmd == IDCANCEL)
                    { EndDialog(hDlg, nCmd); }
                else if (nCmd == IDC_OPTIONS)
                    { s_pThis->OnCmdOptions(); }
                else if (nCmd == IDC_CANCEL_SPELLCHECK)
                    { s_pThis->OnCmdCancelSpellCheck(); }
                else if (nCmd == IDC_RESTART_SPELLCHECK)
                    { s_pThis->OnCmdRestartSpellCheck(); }
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
        case LBN_SELCHANGE:
            {
                s_pThis->OnListBoxSelChange(reinterpret_cast<HWND>(lParam));

                break;
            }
            break;
        }
    }

    return static_cast<INT_PTR>(bHandled);
}

void CCheckSpellingDlg::OnCmdOptions()
{
    TolonSpellCheck::CSession* pSession = GetSession();

    if (pSession)
    {
        tsc_result r = 0;

        CShowOptionsWindowData sowd;
        sowd.ParentWindow(GetHwnd());

        r = pSession->ShowOptionsWindow(sowd);
        if (FAILED(r))
        {
            ::MessageBox(GetHwnd(), L"Failed to display options window.", L"TolonSpellCheck", MB_OK | MB_ICONEXCLAMATION);
        }
        else
        {
            UpdateTitleBar();
        }
    }
}

void CCheckSpellingDlg::OnCmdCancelSpellCheck()
{
    m_checker.StopSpellCheck();
}

void CCheckSpellingDlg::OnCmdIgnoreOnce()
{
    if (m_bWaitingForInput)
    {
        m_checker.IgnoreAndResume();
        m_bWaitingForInput = false;
    }
}

void CCheckSpellingDlg::OnCmdIgnoreAll()
{
    if (m_bWaitingForInput)
    {
        m_checker.IgnoreAllAndResume();
        m_bWaitingForInput = false;
    }
}

void CCheckSpellingDlg::OnCmdAddToDictionary()
{
    if (m_bWaitingForInput)
    {
        m_checker.AddToDicAndResume();
        m_bWaitingForInput = false;
    }
}

void CCheckSpellingDlg::OnCmdChange()
{
    if (m_bWaitingForInput)
    {
        std::wstring sWord;
        GetChangeToText(sWord);
        m_checker.ChangeAndResume(sWord.c_str());
        m_bWaitingForInput = false;
    }
}

void CCheckSpellingDlg::OnCmdChangeAll()
{
    if (m_bWaitingForInput)
    {
        std::wstring sWord;
        GetChangeToText(sWord);
        m_checker.ChangeAllAndResume(sWord.c_str());
        m_bWaitingForInput = false;
    }
}

void CCheckSpellingDlg::OnCmdRestartSpellCheck()
{
    m_checker.StartSpellCheck(GetRichEditHwnd());
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

void CCheckSpellingDlg::TimerProc(HWND hwnd, UINT /*uMsg*/, UINT idEvent, DWORD /*dwTime*/)
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

    if (    ( m_bWaitingForInput == false ) 
         && ( nState == CRichEditSpellChecker::SpellCheckState_WAITING ) )
    {
        //TSC_CHECKWORD_DATA* pCwd = m_checker.GetCheckWordData();
        CCheckWordData& cwd = m_checker.GetCheckWordData();

        m_checker.SelectCurrentWord();

        HWND hWndEditWord = ::GetDlgItem(GetHwnd(), IDC_EDIT_WORD);
        HWND hWndEditChangeTo = ::GetDlgItem(GetHwnd(), IDC_EDIT_CHANGETO);
        if (hWndEditWord && hWndEditChangeTo)
        {
            std::wstring s = cwd.TestWord();

            if (!s.empty())
            {
                ::SetWindowText(hWndEditWord, s.c_str());
                ::SetWindowText(hWndEditChangeTo, s.c_str());
            }
        }

        HWND hwndList = ::GetDlgItem(GetHwnd(), IDC_SUGGESTION_LIST);
        if (hwndList)
        {
            ::SendMessage(hwndList, LB_RESETCONTENT, 0, 0);

            // Call this->AddResultString(s) for each result string
            cwd.ApplyResults(std::bind1st(std::mem_fun(&CCheckSpellingDlg::AddResultString), this));
        }

        m_bWaitingForInput = true;
    }
}

void CCheckSpellingDlg::AddResultString(std::wstring s)
{
    if (!s.empty())
    {
        HWND hwndList = ::GetDlgItem(GetHwnd(), IDC_SUGGESTION_LIST);
        ::SendMessage(hwndList, LB_ADDSTRING, 0, reinterpret_cast<long>(s.c_str()));
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
    case CRichEditSpellChecker::SpellCheckState_IDLE:
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
    EnableDialogItem(IDC_CANCEL_SPELLCHECK, bAllowSpellCheckCancel);

    // General user input controls
    EnableDialogItem(IDC_IGNORE_ONCE, bAllowGeneralInput);
    EnableDialogItem(IDC_IGNORE_ALL, bAllowGeneralInput);
    EnableDialogItem(IDC_ADD_TO_DICTIONARY, bAllowGeneralInput);
    EnableDialogItem(IDC_CHANGE, bAllowGeneralInput);
    EnableDialogItem(IDC_CHANGE_ALL, bAllowGeneralInput);
    EnableDialogItem(IDC_SUGGESTION_LIST, bAllowGeneralInput);
    EnableDialogItem(IDC_EDIT_WORD, bAllowGeneralInput);
    EnableDialogItem(IDC_EDIT_CHANGETO, bAllowGeneralInput);
}

void CCheckSpellingDlg::EnableDialogItem(int nItem, BOOL bEnable)
{
    HWND hwnd = ::GetDlgItem(GetHwnd(), nItem);
    if (hwnd)
    {
        ::EnableWindow(hwnd, bEnable);
    }
}

bool CCheckSpellingDlg::OnListBoxSelChange(HWND hwndListBox)
{
    bool bHandled = false;
    const HWND hwndSuggestions = ::GetDlgItem(GetHwnd(), IDC_SUGGESTION_LIST);

    if (hwndListBox == hwndSuggestions)
    {
        std::wstring sText;
        CWndUtils::GetListBoxCurSelText(hwndSuggestions, sText);
        if (!sText.empty())
        {
            SetChangeToText(sText.c_str());
        }

        bHandled = true;
    }

    return bHandled;
}

void CCheckSpellingDlg::GetChangeToText(std::wstring& sWord)
{
    CWndUtils::GetDlgItemText(GetHwnd(), IDC_EDIT_CHANGETO, sWord);
}

void CCheckSpellingDlg::SetChangeToText(const wchar_t* sWord)
{
    HWND hwndChangeTo = ::GetDlgItem(GetHwnd(), IDC_EDIT_CHANGETO);
    if (hwndChangeTo && sWord)
    {
        ::SendMessage(hwndChangeTo, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(sWord));
    }
}
