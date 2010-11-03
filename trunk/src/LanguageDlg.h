#pragma once

#include "resource.h"
#include "TolonSpellCheckInternals.h"
#include "tscSession.h" //for CSessionOptions
#include <windows.h>
#include <commctrl.h>
#include <string>

class CLanguageDlg
//! Provides the user interface to choose the current session language.
{
protected:
    static int CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    static bool LangEnumCallback(TolonSpellCheck::LANGUAGE_DESC_WIDEDATA* pData, void* pUserData);

public:
    CLanguageDlg(TolonSpellCheck::CSessionOptions& options, HWND hwndParent = NULL);   // standard constructor
    ~CLanguageDlg();

// Dialog Data
    enum { IDD = IDD_DIC_LANG };
    
    int DoModal();

private:
    // Windows message handlers
    void OnCommand(int id, HWND hwndCtl, UINT codeNotify);
    BOOL OnNotify(UINT_PTR idFrom, UINT code, const LPNMHDR lpnmh);

    // Message Handlers
    BOOL OnInitDialog();
    void OnCmdOk();
    void OnCmdCancel();
    void OnCmdMakeDefault();
    void OnDicListColClick(const NMLISTVIEW* pnmlv);

    void InitLangList();

    void GetChosenLanguage(std::string& sLang);

    // User Interface Updating
    //! Returns the window handle for the specified dialog component.
    HWND GetDlgItem(int nDlgItem) const;

    //! Returns the window handle for this window.
    HWND GetHwnd() const;

    //! Returns the window handle for the language list.
    HWND GetLangListHwnd() const;

    //! Reads the language string from the current session and displays 
    //! it in the IDC_DEFAULTLANG_STATIC control.
    void UpdateLanguageDisplay();

    //! The comparison function for sorting this control.
    //! @param lParam1 The list view index of the first item to compare.
    //! @param lParam2 The list view index of the second item to compare.
    //! @param lParamSort Indicates the order of the sort; ascending or descending.
    static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort); 

private:
    TolonSpellCheck::CSessionOptions& m_options;
    //TolonSpellCheck::CSession* const m_pSession; // to be removed
    HWND m_hwnd;
    HWND m_hwndParent;
    HWND m_hwndLangList;

    struct {
        int nCol;
        bool bAsc;
    } m_SortOptions;
};
