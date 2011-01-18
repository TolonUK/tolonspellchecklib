#ifndef __TOLON_WNDUTILS_H__INCLUDED
#define __TOLON_WNDUTILS_H__INCLUDED

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>

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
            int nTextLen = ::GetWindowTextLength(hCtrl);

            if (nTextLen > 0)
            {
                nTextLen = nTextLen + 1;
                std::vector<wchar_t> vText(nTextLen, L'\0');
                ::GetWindowText(hCtrl, &(*vText.begin()), vText.size());
                sText.assign(&(*vText.begin()));
            }
        }
    }

    static void GetListViewItemText(HWND hCtrl, int nItem, int nSubItem, std::wstring& s)
    {
        const int nBufLen = 1024;
        static wchar_t wszBuf[nBufLen];
        
        wszBuf[0] = L'\0';

        ListView_GetItemText( hCtrl,
                              nItem,
                              nSubItem,
                              &wszBuf[0],
                              nBufLen );

        s.assign(wszBuf);
    }

    static void SetDlgItemText( HWND hDlg, int nCtrlID, const wchar_t* sText, const wchar_t* sPrefix )
    {
        HWND hCtrl = ::GetDlgItem(hDlg, IDC_DEFAULTLANG_STATIC);
        if (hCtrl)
        {
            std::wstring ws;
            if (sPrefix)
                ws.assign(sPrefix);
            ws.append(sText);
            ::SetWindowText(hCtrl, ws.c_str());
        }
    }

    static void GetListBoxCurSelText( HWND hCtrl, std::wstring& sText )
    {
        sText.clear();
        const int nSel = ::SendMessage(hCtrl, LB_GETCURSEL, 0, 0);
        if (nSel != LB_ERR)
        {
            int nTextLen = ::SendMessage(hCtrl, LB_GETTEXTLEN, nSel, 0);
            if (nTextLen > 0)
            {
                nTextLen = nTextLen + 1;
                std::vector<wchar_t> vString(nTextLen);
                nTextLen = ::SendMessage(hCtrl, LB_GETTEXT, nSel, reinterpret_cast<LPARAM>(&(*vString.begin())));
                if (nTextLen > 0)
                {
                    sText = &(*vString.begin());
                }
            }
        }
    }
};

#endif __TOLON_WNDUTILS_H__INCLUDED
