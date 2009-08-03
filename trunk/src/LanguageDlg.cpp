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

static enum COLUMNS {
    COL_CODE,
    COL_NAME,
    
    COLUMN_COUNT
};

static struct COLUMN_DATA {
    int Id;
    int Width;
    const wchar_t* Name;
} s_columns[COLUMN_COUNT] {
    { COL_CODE, 80,                       L"Code" },
    { COL_NAME, LVSCW_AUTOSIZE_USEHEADER, L"Language" }
};

static const int s_nColDisplayName;
static const int s_nColCodeName;

// CLanguageDlg dialog

static CLanguageDlg* s_pThis;

bool CLanguageDlg::LangEnumCallback(LANGUAGE_DESC_WIDEDATA* pData, void* pUserData)
{
	bool bResult = false;
	
	CLanguageDlg* pThis = reinterpret_cast<CLanguageDlg*>(pUserData);
	
	if (pData && pThis)
	{
		LVITEM lvi = {0};
		
		lvi.mask = LVIF_TEXT;
		lvi.iItem = 0;
        
        lvi.iSubItem = COL_CODE;
        lvi.pszText = pData->wszCodeName;
        ListView_InsertItem(pThis->m_hwndLangList, &lvi);
        
		ListView_SetItemText(pThis->m_hwndLangList, lvi.iItem, COL_NAME, pData->wszDisplayName);
		
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
	LVCOLUMN lvc = {0};
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    
    for ( int i = 0; i < COLUMN_COUNT; ++i )
    {
        lvc.pszText = s_columns.Name;
        lvc.cx = s_columns.Width;
        ListView_InsertColumn(m_hwndLangList, s_columns.id, &lvc);
    }
}

int CALLBACK CLanguageDlg::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    BOOL bHandled = FALSE;
    
	if (!s_pThis)
		return 0;
	
	if (!s_pThis->m_hwnd)
		s_pThis->m_hwnd = hDlg;
	
	switch (message)
	{
	case WM_INITDIALOG:
		return s_pThis->OnInitDialog();

	case WM_COMMAND:
        const WORD nCmd = LOWORD(wParam);
        
		if (nCmd == IDOK)
        { 
            OnCmdOk();
            EndDialog(hDlg, nCmd);
        }
        else if (nCmd == IDCANCEL)
		{
            OnCmdCancel();
            EndDialog(hDlg, nCmd);
        }
        else if (nCmd == IDC_MAKEDEFAULT_BTN)
        { OnCmdMakeDefault(); }
        
        bHandled = TRUE;
        
		break;
	}
    
	return bHandled;
}

void CLanguageDlg::OnOk()
{
    if (!m_pSession)
    {
        ::MessageBox(GetHwnd(), L"Internal Error. Could not set the chosen dictionary.", L"TolonSpellCheck", MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    else
    {
        // Get chosen language
        // Set it on the session
    }
}

void CLanguageDlg::OnCancel()
{
}

void CLanguageDlg::OnMakeDefault()
{
}
