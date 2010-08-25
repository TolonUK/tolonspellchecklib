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
} s_columns[COLUMN_COUNT] = {
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
    UpdateLanguageDisplay();
	
	return TRUE;
}

void CLanguageDlg::InitLangList()
{
	assert(m_hwnd);
	
	if (!m_hwnd)
		return;
	
	// Obtain HWND
	m_hwndLangList = GetDlgItem(IDC_DIC_LIST);

    // Set styles
    ListView_SetExtendedListViewStyleEx(m_hwndLangList, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	
	// Set up columns
	LVCOLUMN lvc = {0};
	lvc.mask = LVCF_TEXT;
	int nItem = 0;
    
    for ( int i = 0; i < COLUMN_COUNT; ++i )
    {
        lvc.pszText = const_cast<wchar_t*>(s_columns[i].Name);
        nItem = ListView_InsertColumn(m_hwndLangList, s_columns[i].Id, &lvc);
		ListView_SetColumnWidth(m_hwndLangList, nItem, s_columns[i].Width);
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
            s_pThis->OnCmdOk();
            EndDialog(hDlg, nCmd);
        }
        else if (nCmd == IDCANCEL)
		{
            s_pThis->OnCmdCancel();
            EndDialog(hDlg, nCmd);
        }
        else if (nCmd == IDC_MAKEDEFAULT_BTN)
        { s_pThis->OnCmdMakeDefault(); }
        
        bHandled = TRUE;
        
		break;
	}
    
	return bHandled;
}

void CLanguageDlg::OnCmdOk()
{
    TolonSpellCheck::CSession* pSession = GetSession();

    if (!pSession)
    {
        ::MessageBox(GetHwnd(), L"Internal Error. Could not set the chosen dictionary.", L"TolonSpellCheck", MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    else
    {
        std::string sChosenLang;

        // Get chosen language
        GetChosenLanguage(sChosenLang);
        
        // Set it on the session
        tsc_result r = TSC_E_FAIL;
        r = pSession->SetLanguage(sChosenLang.c_str());

        if (TSC_FAILED(r))
        {
            ::MessageBox(GetHwnd(), L"Failed to set new language. :(", L"TolonSpellCheckLib", MB_OK | MB_ICONEXCLAMATION);
        }
    }
}

void CLanguageDlg::OnCmdCancel()
{
}

void CLanguageDlg::OnCmdMakeDefault()
{
}

void CLanguageDlg::GetChosenLanguage(std::string& sLang)
{
    int nItem = -1;
    nItem = ListView_GetNextItem(m_hwndLangList, -1, LVNI_SELECTED);

    if (nItem != -1)
    {
        const int nBufLen = 13;
        wchar_t wszBuf[nBufLen] = L"\0";

        ListView_GetItemText( m_hwndLangList,
                              nItem,
                              COL_CODE,
                              &wszBuf[0],
                              nBufLen );

        char szBuffer[nBufLen] = "\0";
        ::WideCharToMultiByte(CP_UTF8, 0, wszBuf, -1, szBuffer, nBufLen, NULL, NULL);

        sLang.assign(szBuffer);
    }
}

void CLanguageDlg::UpdateLanguageDisplay()
{
    TolonSpellCheck::CSession* pSession = GetSession();

    if (pSession)
    {
        tsc_result result = TSC_S_OK;

        LANGUAGE_DESC_WIDEDATA ldwd;
	wchar_t wszLang[13];
        memset(&ldwd, 0, sizeof(LANGUAGE_DESC_WIDEDATA));
	memset(wszLang, 0, sizeof(wszLang));
	    
	ldwd.cbSize = sizeof(LANGUAGE_DESC_WIDEDATA);    
	result = pSession->GetCurrentLanguage(wszLang);
	
	if (TSC_SUCCEEDED(result))
        {
		    result = m_pSession->DescribeLanguage(wszLang, &ldwd);
    		
		    if (TSC_SUCCEEDED(result))
		    {
			    HWND hwnd = GetDlgItem(IDC_DEFAULTLANG_STATIC);
			    if (hwnd)
			    {
				    wstring ws(L"Default: ");
				    ws.append(ldwd.wszDisplayName);
				    SetWindowText(hwnd, ws.c_str());
			    }
		    }
	    }
    }
}

HWND CLanguageDlg::GetDlgItem(int nDlgItem) const
{
    HWND hMain = GetHwnd();
    HWND hItem = NULL;

    if (hMain)
    {
        hItem = ::GetDlgItem(hMain, nDlgItem);
    }

    assert(hItem != NULL);
    return hItem;
}

HWND CLanguageDlg::GetHwnd() const
{ 
    assert(m_hwnd != NULL);
    return m_hwnd;
}

TolonSpellCheck::CSession* CLanguageDlg::GetSession() const
{ 
    assert(m_pSession != NULL);
    return m_pSession;
}

int CALLBACK CLanguageDlg::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    return 0;
}
