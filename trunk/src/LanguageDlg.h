#pragma once

#include "resource.h"
#include "TolonSpellCheckInternals.h"

// CLanguageDlg dialog

class CLanguageDlg : public CDialog
{
	DECLARE_DYNAMIC(CLanguageDlg)

public:
	CLanguageDlg(TolonSpellCheck::CSession* pSession, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLanguageDlg();

// Dialog Data
	enum { IDD = IDD_DIC_LANG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

protected:
    TolonSpellCheck::CSession* const m_pSession;
};
