#ifndef __TOLON_WNDUTILS_H__INCLUDED
#define __TOLON_WNDUTILS_H__INCLUDED

#include <windows.h>

class CWndUtils
{
public:
    static BOOL CheckDlgButton(HWND hDlg, int nIDButton, bool bCheck)
        { return ::CheckDlgButton(hDlg, nIDButton, bCheck ? BST_CHECKED : BST_UNCHECKED); }

    static bool IsDlgButtonChecked(HWND hDlg, int nIDButton)
        { return ::IsDlgButtonChecked(hDlg, nIDButton) != BST_UNCHECKED; }
        
    static void GetDlgItemText(HWND hDlg, int nID, std::wstring& sText)
    {
        const HWND hCtrl = ::GetDlgItem(hDlg, nID);
        sText.clear();
        
        if (hCtrl)
        {
            const int nTextLen = ::GetWindowTextLength(hCtrl);

            if (nTextLen > 0)
            {
                nTextLen = nTextLen + 1;
                std::vector<wchar_t> vText(nTextLen, L'\0');
                ::GetWindowText(hCtrl, &(*vText.begin()), vText.size());
                sText.assign(&(*vText.begin()));
            }
        }
    }
};

#endif __TOLON_WNDUTILS_H__INCLUDED
