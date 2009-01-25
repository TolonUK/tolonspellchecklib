// LanguageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageDlg.h"
#include "TolonSpellCheckInternals.h"
#include "tscSession.h"

using namespace TolonSpellCheck;

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
    
    tsc_result result = TSC_S_OK;
    wchar_t wszLang[13];
    LANGUAGE_DESC_WIDEDATA ldwd;
    memset(wszLang, 0, sizeof(wszLang));
    memset(&ldwd, 0, sizeof(LANGUAGE_DESC_WIDEDATA));
    
    result = m_pSession->GetCurrentLanguage(wszLang);
    
    if (SUCCEEDED(result))
    {
        AfxMessageBox(wszLang);
        
        result = m_pSession->DescribeLanguage(wszLang, &ldwd);
        
        if (SUCCEEDED(result))
        {
            AfxMessageBox(ldwd.wszDisplayName);
        }
    }
    
    return TRUE;
}
