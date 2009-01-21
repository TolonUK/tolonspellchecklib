#pragma once

#include "resource.h"

// CCheckSpellingDlg dialog

class CCheckSpellingDlg : public CDialog
{
	DECLARE_DYNAMIC(CCheckSpellingDlg)

public:
	CCheckSpellingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCheckSpellingDlg();

// Dialog Data
	enum { IDD = IDD_CHECK_SPELLING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
