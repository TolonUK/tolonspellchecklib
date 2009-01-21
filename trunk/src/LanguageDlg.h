#pragma once

#include "resource.h"

// CLanguageDlg dialog

class CLanguageDlg : public CDialog
{
	DECLARE_DYNAMIC(CLanguageDlg)

public:
	CLanguageDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLanguageDlg();

// Dialog Data
	enum { IDD = IDD_DIC_LANG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
