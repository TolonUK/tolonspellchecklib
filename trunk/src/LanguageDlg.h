#pragma once

#include "resource.h"
#include "TolonSpellCheckInternals.h"
#include <string>

class CLanguageDlg
//! Provides the user interface to choose the current session language.
{
protected:
    static int CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    static bool LangEnumCallback(TolonSpellCheck::LANGUAGE_DESC_WIDEDATA* pData, void* pUserData);

public:
    CLanguageDlg(TolonSpellCheck::CSession* pSession, HWND hwndParent = NULL);   // standard constructor
    ~CLanguageDlg();

// Dialog Data
    enum { IDD = IDD_DIC_LANG };
    
    int DoModal();

private:
    // Message Handlers
    BOOL OnInitDialog();
    void OnCmdOk();
    void OnCmdCancel();
    void OnCmdMakeDefault();

    void InitLangList();

    void GetChosenLanguage(std::string& sLang);

    //! Returns the TolonSpellCheck session object.
    TolonSpellCheck::CSession* GetSession() const;

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
    TolonSpellCheck::CSession* const m_pSession;
    HWND m_hwnd;
    HWND m_hwndParent;
    HWND m_hwndLangList;
};
