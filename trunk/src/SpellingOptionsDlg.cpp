// SpellingOptions.cpp : implementation file
//

#include "stdafx.h"
#include "SpellingOptionsDlg.h"
#include "LanguageDlg.h"

// CSpellingOptionsDlg dialog

IMPLEMENT_DYNAMIC(CSpellingOptionsDlg, CDialog)

CSpellingOptionsDlg::CSpellingOptionsDlg(TolonSpellCheck::CSession* pSession, CWnd* pParent /*=NULL*/) :
    CDialog(CSpellingOptionsDlg::IDD, pParent),
    m_pSession(pSession)
{

}

CSpellingOptionsDlg::~CSpellingOptionsDlg()
{
}

void CSpellingOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSpellingOptionsDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, &CSpellingOptionsDlg::OnDicLangClicked)
END_MESSAGE_MAP()


// CSpellingOptionsDlg message handlers
BOOL CSpellingOptionsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    
    return TRUE;
}

void CSpellingOptionsDlg::OnDicLangClicked()
{
    CLanguageDlg dlg(m_pSession);
    dlg.DoModal();
}
