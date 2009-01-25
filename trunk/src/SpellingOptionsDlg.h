#pragma once

#include <afxwin.h>
#include "resource.h"
#include "TolonSpellCheckInternals.h"

// CSpellingOptionsDlg dialog

class CSpellingOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpellingOptionsDlg)

public:
	CSpellingOptionsDlg(TolonSpellCheck::CSession* pSession, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpellingOptionsDlg();

// Dialog Data
	enum { IDD = IDD_SPELLING_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    virtual BOOL OnInitDialog();

public:
    afx_msg void OnDicLangClicked();

	DECLARE_MESSAGE_MAP()

protected:
    TolonSpellCheck::CSession* const m_pSession;
};
