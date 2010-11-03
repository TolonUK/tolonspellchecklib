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
};

#endif __TOLON_WNDUTILS_H__INCLUDED
