// LanguageDlg.cpp : implementation file
//

#include "LanguageDlg.h"
#include "TolonSpellCheckInternals.h"
#include "tscSession.h"
#include <iostream>
#include <string>
#include <sstream>
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

//static const int s_nColDisplayName;
//static const int s_nColCodeName;

static const bool s_bDefaultSortAsc = true;

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
        ListView_InsertItem(pThis->GetLangListHwnd(), &lvi);
        
        ListView_SetItemText(pThis->GetLangListHwnd(), lvi.iItem, COL_NAME, pData->wszDisplayName);
        
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
    m_SortOptions.bAsc = s_bDefaultSortAsc;
    m_SortOptions.nCol = COL_NAME;
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
    
    // Fix up language list now it's populated with data
    ListView_SetColumnWidth(GetLangListHwnd(), 0, LVSCW_AUTOSIZE);
    ListView_SortItemsEx(GetLangListHwnd(), &CompareFunc,  reinterpret_cast<LPARAM>(this));
    
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
    ListView_SetExtendedListViewStyleEx(GetLangListHwnd(), LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
    
    // Set up columns
    LVCOLUMN lvc = {0};
    lvc.mask = LVCF_TEXT;
    int nItem = 0;
    
    for ( int i = 0; i < COLUMN_COUNT; ++i )
    {
        lvc.pszText = const_cast<wchar_t*>(s_columns[i].Name);
        nItem = ListView_InsertColumn(GetLangListHwnd(), s_columns[i].Id, &lvc);
        ListView_SetColumnWidth(GetLangListHwnd(), nItem, s_columns[i].Width);
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
    case WM_COMMAND:
        {
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

    case WM_INITDIALOG:
        return s_pThis->OnInitDialog();

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case LVN_COLUMNCLICK:
            if (((LPNMHDR)lParam)->idFrom == IDC_DIC_LIST)
            {
                s_pThis->OnDicListColClick(lParam);
                return TRUE;
            }
            break; 
        break;
        }
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
        string sChosenLang;

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

void CLanguageDlg::GetChosenLanguage(string& sLang)
{
    int nItem = -1;
    nItem = ListView_GetNextItem(GetLangListHwnd(), -1, LVNI_SELECTED);

    if (nItem != -1)
    {
        const int nBufLen = 13;
        wchar_t wszBuf[nBufLen] = L"\0";

        ListView_GetItemText( GetLangListHwnd(),
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

HWND CLanguageDlg::GetLangListHwnd() const
{ 
    assert(m_hwndLangList != NULL);
    return m_hwndLangList;
}

TolonSpellCheck::CSession* CLanguageDlg::GetSession() const
{ 
    assert(m_pSession != NULL);
    return m_pSession;
}

//! @param lParam1 The list view index of the first item to compare.
//! @param lParam2 The list view index of the second item to compare.
//! @param lParamSort Indicates the order of the sort; ascending or descending.
int CALLBACK CLanguageDlg::CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort ) 
{
    CLanguageDlg* pThis = reinterpret_cast<CLanguageDlg*>(lParamSort);
    const size_t nTextBufferLength = 64;
    static wchar_t sText1[nTextBufferLength], sText2[nTextBufferLength];
    
    const int nSortColumn = pThis->m_SortOptions.nCol;
    const bool bAsc = pThis->m_SortOptions.bAsc;
    
    sText1[0] = L'\0';
    ListView_GetItemText( pThis->GetLangListHwnd(), lParam1, nSortColumn, sText1, nTextBufferLength );
    sText2[0] = L'\0';
    ListView_GetItemText( pThis->GetLangListHwnd(), lParam2, nSortColumn, sText2, nTextBufferLength );
    
    // We're dealing with a windows user interface, so use the Win32 string comparison.
    int nComp = CompareString( LOCALE_USER_DEFAULT, NORM_IGNORECASE, sText1, wcslen(sText1), sText2, wcslen(sText2) );

    // CompareString documentation says substract 2 to return standard <0, ==0, and >0 values.
    nComp = nComp - 2;
    if (bAsc)
        return -nComp;
    else
        return nComp;
}

void CLanguageDlg::OnDicListColClick(LPARAM lParam)
{
    const NMLISTVIEW* pnmlv = reinterpret_cast<const NMLISTVIEW*>(lParam);

    if (pnmlv->iSubItem == m_SortOptions.nCol)
    {
        m_SortOptions.bAsc = !m_SortOptions.bAsc;
    }
    else
    {
        m_SortOptions.nCol = pnmlv->iSubItem;
    }

    ListView_SortItemsEx(GetLangListHwnd(), &CompareFunc,  reinterpret_cast<LPARAM>(this));
}
