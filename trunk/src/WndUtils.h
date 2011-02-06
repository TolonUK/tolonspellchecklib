#ifndef __TOLON_WNDUTILS_H__INCLUDED
#define __TOLON_WNDUTILS_H__INCLUDED

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>

namespace TolonUI { namespace Windows {

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

    static void SetDlgItemText( HWND hDlg, int nCtrlID, const wchar_t* sText, const wchar_t* sPrefix )
    {
        HWND hCtrl = ::GetDlgItem(hDlg, nCtrlID);
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
        if (!hCtrl)
            return;

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

    static void SetWindowText( HWND hWnd, const wchar_t* sText, const wchar_t* sPrefix )
    {
        if (!hWnd)
            return;

        std::wstring ws;
        if (sPrefix)
            ws.assign(sPrefix);
        ws.append(sText);
        ::SetWindowText(hWnd, ws.c_str());
    }
};

class CListViewUtils
{
public:

    static void GetListViewItemText(HWND hCtrl, int nItem, int nSubItem, std::wstring& s)
    {
        if (!hCtrl)
            return;

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

    static void SingleSelectListViewItem( HWND hCtrl, int i )
    {
        if (!hCtrl)
            return;

        int iCur = -1;

        // De-select any selected items
        do
        {
            iCur = ListView_GetNextItem(hCtrl, iCur, LVNI_SELECTED);

            if ( (iCur != -1) && (iCur != i) )
            {
                ListView_SetItemState(hCtrl, iCur, 0, LVIS_SELECTED);
            }
        } while (iCur != -1);

        // Remove any existing focus
        iCur = ListView_GetNextItem(hCtrl, -1, LVNI_FOCUSED);
        if ( (iCur != -1) && (iCur != i) )
        {
            ListView_SetItemState(hCtrl, iCur, 0, LVIS_FOCUSED);
        }

        ListView_SetItemState(hCtrl, i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    }

    static void EnsureVisiblePreferMiddle( HWND hCtrl, int i )
    {
        const int iCountPerPage = ListView_GetCountPerPage(hCtrl) + 1;
        const int iDelta = (iCountPerPage - 1) / 2;

        // First ensure the primary item is visible
        ListView_EnsureVisible(hCtrl, i, 0);

        // Then try the items at the top and bottom of the page
        if (iDelta > 0)
        {
            int iTop = i - iDelta;
            if (iTop < 0)
                iTop = 0;
            if (iTop != i)
                ListView_EnsureVisible(hCtrl, iTop, 0);

            int iBottom = i + iDelta;
            const int iItemCount = ListView_GetItemCount(hCtrl);
            if (iBottom >= iItemCount)
                iBottom = iItemCount - 1;
            if (iBottom != i)
                ListView_EnsureVisible(hCtrl, iBottom, 0);
        }
    }

    static void AutoSizeAllColumns( HWND hCtrl )
    {
        if (!hCtrl)
            return;

        HWND hHeader = ListView_GetHeader(hCtrl);

        if (hHeader)
        {
            int iColCount = Header_GetItemCount(hHeader);
            if (iColCount > 0)
            {
                for (int i = 0; i < (iColCount - 1); ++i)
                {
                    ListView_SetColumnWidth(hCtrl, 0, LVSCW_AUTOSIZE);
                }

                ListView_SetColumnWidth(hCtrl, iColCount - 1, LVSCW_AUTOSIZE_USEHEADER);
            }
        }
    }
};

} } // namespace TolonUI.Windows

#endif __TOLON_WNDUTILS_H__INCLUDED
