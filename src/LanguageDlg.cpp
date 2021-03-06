// LanguageDlg.cpp : implementation file
//

#include "LanguageDlg.h"
#include "isoLang.h"
#include "TolonSpellCheckInternals.h"
#include "tscSession.h"
#include "tscModule.h"
#include "WndUtils.h"
#include <iostream>
#include <string>
#include <sstream>
#include <windowsx.h>
#include <assert.h>

extern HINSTANCE g_hInstDll;

using namespace TolonSpellCheck;
using namespace TolonUI::Windows;
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

CLanguageDlg::CLanguageDlg(TolonSpellCheck::CSessionOptionsData& options, HWND hwndParent /*=NULL*/) :
    //m_pSession(pSession),
    m_options(options),
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
    return DialogBox(g_hInstDll, MAKEINTRESOURCE(CLanguageDlg::IDD), m_hwndParent, CLanguageDlg::WndProc);
}

// CLanguageDlg message handlers
BOOL CLanguageDlg::OnInitDialog()
{
    // Sort out controls
    InitLangList();
    
    // Populate language list
    CModule* pMod = CModule::GetInstance();
    if (!pMod)
        return FALSE;

    pMod->EnumLanguages(static_cast<LanguageEnumFn>(&CLanguageDlg::LangEnumCallback), this);
    
    // Fix up language list now it's populated with data
    CListViewUtils::AutoSizeAllColumns(GetLangListHwnd());
    ListView_SortItemsEx(GetLangListHwnd(), &CompareFunc,  reinterpret_cast<LPARAM>(this));
    
    // Update UI with current language
    SelectCurrentLanguage();

    // Update UI with default language
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
        s_pThis->OnCommand(static_cast<int>(LOWORD(wParam)), reinterpret_cast<HWND>(lParam), static_cast<UINT>(HIWORD(wParam)));
        bHandled = TRUE;
        break;

    case WM_INITDIALOG:
        bHandled = s_pThis->OnInitDialog();
        break;

    case WM_NOTIFY:
        LPNMHDR pnmh = reinterpret_cast<LPNMHDR>(lParam);
        bHandled = s_pThis->OnNotify(pnmh->idFrom, pnmh->code, pnmh);
        break;
    }
    
    return bHandled;
}

void CLanguageDlg::OnCmdOk()
{
    wstring sChosenLang;

    // Get chosen language
    GetChosenLanguage(sChosenLang);
    
    if (sChosenLang.empty())
    {
        ::MessageBox(GetHwnd(), L"No language chosen. :(", L"TolonSpellCheckLib", MB_OK | MB_ICONEXCLAMATION);
    }
    else
    {
        m_options.CurrentLanguage(sChosenLang.c_str());
    }
}

void CLanguageDlg::OnCmdCancel()
{
}

void CLanguageDlg::OnCmdMakeDefault()
{
    wstring sChosenLang;

    // Get chosen language
    GetChosenLanguage(sChosenLang);

    m_options.DefaultLanguage(sChosenLang.c_str());

    UpdateLanguageDisplay();
}

void CLanguageDlg::GetChosenLanguage(wstring& sLang)
{
    int nItem = LB_ERR;
    nItem = ListView_GetNextItem(GetLangListHwnd(), -1, LVNI_SELECTED);

    if (nItem == LB_ERR)
    {
        sLang.clear();
    }
    else
    {
        CListViewUtils::GetListViewItemText( GetLangListHwnd(),
                                             nItem,
                                             COL_CODE,
                                             sLang );
    }
}

void CLanguageDlg::SelectCurrentLanguage()
{
    //NEED TO GET THE CURRENT LANGUAGE HERE
    const wchar_t* sCurrent = m_options.CurrentLanguage();
    HWND hCtrl = GetLangListHwnd();
    assert(sCurrent && hCtrl);
    if (sCurrent && hCtrl)
    {
        const int nItemCount = ListView_GetItemCount(hCtrl);
        std::wstring sItemCode;

        for (int i = 0; i < nItemCount; ++i)
        {
            CListViewUtils::GetListViewItemText(hCtrl, i, COL_CODE, sItemCode);
            if (!sItemCode.empty())
            {
                if (CIsoLang::Compare(sCurrent, sItemCode.c_str()) == 0)
                {
                    CListViewUtils::SingleSelectListViewItem(hCtrl, i);
                    CListViewUtils::EnsureVisiblePreferMiddle(hCtrl, i);
                    break;
                }
            }
        }
    }
}

void CLanguageDlg::UpdateLanguageDisplay()
{
    const wchar_t* sDefault = m_options.DefaultLanguage();
    
    assert(sDefault);
    if (sDefault)
    {
        std::wstring sDisplay(sDefault);
        CIsoLang::GetDisplayName(sDefault, sDisplay);
            
        CWndUtils::SetDlgItemText( GetHwnd(),
                                   IDC_DEFAULTLANG_STATIC,
                                   sDisplay.c_str(),
                                   L"Default: " );
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
        return nComp;
    else
        return -nComp;
}

void CLanguageDlg::OnDicListColClick(const NMLISTVIEW* pnmlv)
{
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

void CLanguageDlg::OnCommand(int id, HWND /*hwndCtl*/, UINT /*codeNotify*/)
{
    if (id == IDOK)
    { 
        OnCmdOk();
        EndDialog(GetHwnd(), id);
    }
    else if (id == IDCANCEL)
    {
        OnCmdCancel();
        EndDialog(GetHwnd(), id);
    }
    else if (id == IDC_MAKEDEFAULT_BTN)
    { OnCmdMakeDefault(); }
}

BOOL CLanguageDlg::OnNotify(UINT_PTR idFrom, UINT code, const LPNMHDR lpnmh)
{
    BOOL bHandled = FALSE;

    switch (code)
    {
    case LVN_COLUMNCLICK:
        if (idFrom == IDC_DIC_LIST)
        {
            const NMLISTVIEW* pnmlv = reinterpret_cast<const NMLISTVIEW*>(lpnmh);
            s_pThis->OnDicListColClick(pnmlv);
            bHandled = TRUE;
        }
        break; 
    }

    return bHandled;
}
