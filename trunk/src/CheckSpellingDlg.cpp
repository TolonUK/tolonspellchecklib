#include "CheckSpellingDlg.h"
#include <windows.h>
#include <assert.h>

// CCheckSpellingDlg dialog

extern HINSTANCE g_hInstDll;

using namespace TolonSpellCheck;

static CCheckSpellingDlg* s_pThis;

CCheckSpellingDlg::CCheckSpellingDlg(HWND hwndParent /*=NULL*/) :
	m_hwnd(NULL),
    m_hwndParent(hwndParent)
{

}

CCheckSpellingDlg::~CCheckSpellingDlg()
{
}

int CCheckSpellingDlg::DoModal()
{
	s_pThis = this;
	DialogBox(g_hInstDll, MAKEINTRESOURCE(CCheckSpellingDlg::IDD), m_hwndParent, CCheckSpellingDlg::WndProc);
	return 0;
}

BOOL CCheckSpellingDlg::OnInitDialog()
{
	return TRUE;
}

int CALLBACK CCheckSpellingDlg::WndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!s_pThis)
		return 0;
	
	if (!s_pThis->m_hwnd)
		s_pThis->m_hwnd = hDlg;
	
	switch (message)
	{
	case WM_INITDIALOG:
		return s_pThis->OnInitDialog();

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}