// LanguageDlg.cpp : implementation file
//

#include "LanguageDlg.h"
#include "TolonSpellCheckInternals.h"
#include "tscSession.h"
#include <string>
#include <windows.h>
#include <commctrl.h>
#include <assert.h>

extern HINSTANCE g_hInstDll;

using namespace TolonSpellCheck;
using namespace std;

// CLanguageDlg dialog

static CLanguageDlg* s_pThis;

bool CLanguageDlg::LangEnumCallback(LANGUAGE_DESC_WIDEDATA* pData, void* pUserData)
{
	bool bResult = false;
	
	CLanguageDlg* pThis = reinterpret_cast<CLanguageDlg*>(pUserData);
	
	if (pData && pThis)
	{
		LVITEM lvi;
		memset(&lvi, 0, sizeof(lvi));
		
		lvi.mask = LVIF_TEXT;
		lvi.iItem = 0;
		lvi.iSubItem = 0;
		lvi.pszText = pData->wszDisplayName;
		ListView_InsertItem(pThis->m_hwndLangList, &lvi);
		
		bResult = true;
	}
	
	return bResult;
}

CLanguageDlg::CLanguageDlg(TolonSpellCheck::CSession* pSession, HWND hwndParent /*=NULL*/) :
	m_pSession(pSession),
	m_hwnd(NULL),
	m_hwndParent(hwndParent),
	m_hwndLangList(NULL)
{

}

CLanguageDlg::~CLanguageDlg()
{
}

int CLanguageDlg::DoModal()
{
	s_pThis = this;
	DialogBox(g_hInstDll, MAKEINTRESOURCE(CLanguageDlg::IDD), m_hwndParent, CLanguageDlg::WndProc);
	return 0;
}

// CLanguageDlg message handlers
BOOL CLanguageDlg::OnInitDialog()
{
	// Sort out controls
	InitLangList();
	
	// Populate language list
	m_pSession->EnumLanguages(static_cast<LanguageEnumFn>(&CLanguageDlg::LangEnumCallback), this);
	ListView_SetColumnWidth(m_hwndLangList, 0, LVSCW_AUTOSIZE);
	
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
			HWND hwnd = GetDlgItem(m_hwnd, IDC_DEFAULTLANG_STATIC);
			if (hwnd)
			{
				wstring ws(L"Default: ");
				ws.append(ldwd.wszDisplayName);
				SetWindowText(hwnd, ws.c_str());
			}
		}
	}
	
	return TRUE;
}

void CLanguageDlg::InitLangList()
{
	assert(m_hwnd);
	
	if (!m_hwnd)
		return;
	
	// Obtain HWND
	m_hwndLangList = GetDlgItem(m_hwnd, IDC_DIC_LIST);
	
	// Set up columns
	LVCOLUMN lvc;
	memset(&lvc, 0, sizeof(lvc));
	lvc.mask = LVCF_TEXT;
	lvc.pszText = L"Language";
	ListView_InsertColumn(m_hwndLangList, 0, &lvc);
}

int CALLBACK CLanguageDlg::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!s_pThis)
		return 0;
	
	if (!s_pThis->m_hwnd)
		s_pThis->m_hwnd = hDlg;
	
	switch (message)
	{
	case WM_INITDIALOG:
		return s_pThis->OnInitDialog();

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}