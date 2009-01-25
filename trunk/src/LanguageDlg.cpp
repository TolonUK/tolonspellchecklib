// LanguageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageDlg.h"


// CLanguageDlg dialog

IMPLEMENT_DYNAMIC(CLanguageDlg, CDialog)

CLanguageDlg::CLanguageDlg(TolonSpellCheck::CSession* pSession, CWnd* pParent /*=NULL*/) :
	CDialog(CLanguageDlg::IDD, pParent),
    m_pSession(pSession)
{

}

CLanguageDlg::~CLanguageDlg()
{
}

void CLanguageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLanguageDlg, CDialog)
END_MESSAGE_MAP()


// CLanguageDlg message handlers
BOOL CLanguageDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    
    return TRUE;
}
