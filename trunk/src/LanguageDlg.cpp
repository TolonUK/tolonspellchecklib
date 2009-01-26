// LanguageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageDlg.h"
#include "TolonSpellCheckInternals.h"
#include "tscSession.h"
#include <string>

using namespace TolonSpellCheck;
using namespace std;

// CLanguageDlg dialog

IMPLEMENT_DYNAMIC(CLanguageDlg, CDialog)

bool CLanguageDlg::LangEnumCallback(LANGUAGE_DESC_WIDEDATA* pData, void* pUserData)
{
	bool bResult = false;
	
	CLanguageDlg* pThis = reinterpret_cast<CLanguageDlg*>(pUserData);
	
	if (pData && pThis)
	{
		int nItem = 0;
		nItem = pThis->m_wndLangList.InsertItem(0, L"");
		pThis->m_wndLangList.SetItemText(nItem, 0, pData->wszDisplayName);
		bResult = true;
	}
	
	return bResult;
}

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
	
	// Sort out controls
	m_wndLangList.SubclassDlgItem(IDC_DIC_LIST, this);
	m_wndLangList.InsertColumn(0, _T("Language"));
	
	// Populate language list
	m_pSession->EnumLanguages(static_cast<LanguageEnumFn>(&CLanguageDlg::LangEnumCallback), this);
	m_wndLangList.SetColumnWidth(0, LVSCW_AUTOSIZE);
	
	// Get default language
	tsc_result result = TSC_S_OK;
	wchar_t wszLang[13];
	LANGUAGE_DESC_WIDEDATA ldwd;
	memset(wszLang, 0, sizeof(wszLang));
	memset(&ldwd, 0, sizeof(LANGUAGE_DESC_WIDEDATA));
	
	ldwd.cbSize = sizeof(LANGUAGE_DESC_WIDEDATA);    
	result = m_pSession->GetCurrentLanguage(wszLang);
	
	if (SUCCEEDED(result))
	{        
		result = m_pSession->DescribeLanguage(wszLang, &ldwd);
		
		if (SUCCEEDED(result))
		{
			CWnd * pWnd = GetDlgItem(IDC_DEFAULTLANG_STATIC);
			if (pWnd)
			{
				wstring ws(L"Default: ");
				ws.append(ldwd.wszDisplayName);
				pWnd->SetWindowText(ws.c_str());
			}
		}
	}
	
	return TRUE;
}
