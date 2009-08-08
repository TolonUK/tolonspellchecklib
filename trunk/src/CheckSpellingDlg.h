#pragma once

#include "resource.h"
#include "TolonSpellCheckInternals.h"
#include "RichEditSpellChecker.h"

// CCheckSpellingDlg dialog

class CCheckSpellingDlg
{
public:
    CCheckSpellingDlg(TolonSpellCheck::CSession* pSession, TSC_CHECKSPELLING_DATA* pData);
	~CCheckSpellingDlg();

// Dialog Data
	enum { IDD = IDD_CHECK_SPELLING };
	
	int DoModal();

private:
	static int CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

    // Message handlers
	BOOL OnInitDialog();
    void OnCmdOptions();
    void OnCmdCancelSpellCheck();
    void OnCmdIgnoreOnce();
    void OnCmdIgnoreAll();
    void OnCmdAddToDictionary();
    void OnCmdChange();
    void OnCmdChangeAll();
    void OnCmdRestartSpellCheck();

    // Timer handlers
    void OnTimer_PollChecker();

    TolonSpellCheck::CSession* GetSession() const
    { return m_pSession; }

    HWND GetHwnd() const
    { return m_hwnd; }

    HWND GetRichEditHwnd() const;

    void UpdateUI();
    
    void EnableDialogItem(int nDlgItem, BOOL bEnable);

    void UpdateTitleBar();

private:
    TolonSpellCheck::CSession* m_pSession;
    TSC_CHECKSPELLING_DATA* m_pData;
	HWND m_hwnd;
	HWND m_hwndParent;
    TolonSpellCheck::CRichEditSpellChecker m_checker;
    static const UINT_PTR m_nCheckerPollEvent;

    TolonSpellCheck::CRichEditSpellChecker::State m_nLastCheckerState;
    DWORD m_dwLastCheckerCharsDone;
    DWORD m_dwLastCheckerCharsTotal;
    bool m_bWaitingForInput;
};
