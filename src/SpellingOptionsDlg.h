#ifndef _TOLON_SPELLING_OPTIONS_DLG_H__
#define _TOLON_SPELLING_OPTIONS_DLG_H__

#include "resource.h"
#include "TolonSpellCheckInternals.h"
#include "tscSession.h" // for CSessionOptions
#include <boost/noncopyable.hpp>

// CSpellingOptionsDlg dialog

class CSpellingOptionsDlg : public boost::noncopyable
{
protected:
    static int CALLBACK WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

public:
    CSpellingOptionsDlg(TolonSpellCheck::CSessionOptionsData& options, HWND hwndParent = NULL);   // standard constructor
    ~CSpellingOptionsDlg();

// Dialog Data
    enum { IDD = IDD_SPELLING_OPTIONS };
    
    int DoModal();

    HWND GetHwnd() const { return m_hwnd; }

protected:
    BOOL OnInitDialog();

public:
    void OnDicLangClicked();
    void OnOk();

protected:
    TolonSpellCheck::CSessionOptionsData& m_options;
    HWND m_hwnd;
    HWND m_hwndParent;
};

#endif //_TOLON_SPELLING_OPTIONS_DLG_H__
