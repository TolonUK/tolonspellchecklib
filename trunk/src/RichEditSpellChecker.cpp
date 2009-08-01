#include "RichEditSpellChecker.h"
#include <iomanip>
#include <iterator>
#include <locale>
#include <sstream>
#include <strstream>
#include "tscSession.h"
#include <richedit.h>
#include "CheckSpellingDlg.h"

using namespace std;
using namespace TolonSpellCheck;

CRichEditSpellChecker::CRichEditSpellChecker(CSession* pSession) : 
	m_pSession(pSession)
{
}

CRichEditSpellChecker::~CRichEditSpellChecker()
{
}

DWORD CRichEditSpellChecker::RichEditStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CRichEditSpellChecker* pThis = reinterpret_cast<CRichEditSpellChecker*>(dwCookie);
	
	if (pThis)
		return pThis->DoCallbackWork(pbBuff, cb, pcb);
	else
		return static_cast<DWORD>(-1);
}

void CRichEditSpellChecker::CheckSpelling(HWND hwndEditCtrl)
{
	EDITSTREAM es;
	es.dwCookie = reinterpret_cast<DWORD>(this);
	es.dwError = 0;
	es.pfnCallback = &CRichEditSpellChecker::RichEditStreamOutCallback;
	
	PreSpellCheck();
    ::SendMessage(hwndEditCtrl, EM_STREAMOUT, SF_UNICODE | SF_TEXT, reinterpret_cast<LPARAM>(&es));
	PostSpellCheck();
}

void CRichEditSpellChecker::PreSpellCheck()
{
	m_sWord.str(L"");
}

void CRichEditSpellChecker::PostSpellCheck()
{
	// Call this one last time in case we have a word waiting at the end of the stream.
	ProcessWord();
}

DWORD CRichEditSpellChecker::DoCallbackWork(LPBYTE pbBuff, LONG cb, LONG* pcb)
{
    const size_t nWideChars = cb / sizeof(std::wstring::value_type);

    std::wstring ws(reinterpret_cast<wchar_t*>(pbBuff), nWideChars);
    std::wstring::const_iterator it, itEnd;

    it = ws.begin();
    itEnd = ws.end();
	for ( ; it != itEnd; it++ )
	{
        if (IsUnicodeAlpha(*it))
			m_sWord << *it;
		else
			ProcessWord();
	}

    *pcb = nWideChars * sizeof(std::wstring::value_type);
	
	return 0;
}

void CRichEditSpellChecker::ProcessWord()
{
	wstring sWord(m_sWord.str());
    string sUtf8Word;

	m_sWord.str(L"");

    m_oConv.utf8FromUnicode(sWord.c_str(), sUtf8Word);

	if (!sWord.empty())
	{
		tsc_result tr = TSC_E_UNEXPECTED;
        TSC_CHECKWORD_DATA cw = {0};
		
		cw.cbSize = sizeof(cw);
		cw.nWordSize = sUtf8Word.size();
		cw.uTestWord.szWord8 = sUtf8Word.c_str();
		
		tr = m_pSession->CheckWord(&cw);
		
		if (TSC_FAILED(tr))
            ::MessageBox(NULL, L"Failure in CheckWord().", L"TolonSpellCheckLib", MB_OK | MB_ICONEXCLAMATION);
        else if (!cw.bOk)
        {
            ::OutputDebugStringA(cw.uTestWord.szWord8);
            ::OutputDebugStringA(": word not found!!!\n");
            CCheckSpellingDlg dlg;
            dlg.DoModal();
        }
        else
        {
            ::OutputDebugStringA("Word ok\n");
        }
	}
}

bool CRichEditSpellChecker::IsUnicodeAlpha(wchar_t wch)
{
    WORD nCharType = 0;
    ::GetStringTypeEx(LOCALE_USER_DEFAULT, CT_CTYPE1, &wch, 1, &nCharType);
    return (nCharType & C1_ALPHA) != 0;
}
