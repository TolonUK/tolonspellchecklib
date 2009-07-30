#include "RichEditSpellChecker.h"
#include <iomanip>
#include <locale>
#include <sstream>
#include <strstream>
#include "tscSession.h"
#include <richedit.h>
#include "CheckSpellingDlg.h"

#define BOOST_UTF8_BEGIN_NAMESPACE \
     namespace TolonSpellCheck { 
#define BOOST_UTF8_DECL
#define BOOST_UTF8_END_NAMESPACE }
#include <libs/detail/utf8_codecvt_facet.cpp>
#undef BOOST_UTF8_END_NAMESPACE
#undef BOOST_UTF8_DECL
#undef BOOST_UTF8_BEGIN_NAMESPACE

using namespace std;
using namespace boost;
using namespace TolonSpellCheck;

CRichEditSpellChecker::CRichEditSpellChecker(CSession* pSession) : 
	//m_loc("English_UK"),
    m_loc(m_locOld, new utf8_codecvt_facet),
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

#ifndef SF_USECODEPAGE
#define SF_USECODEPAGE	0x0020
#endif

void CRichEditSpellChecker::CheckSpelling(HWND hwndEditCtrl)
{
	EDITSTREAM es;
	es.dwCookie = reinterpret_cast<DWORD>(this);
	es.dwError = 0;
	es.pfnCallback = &CRichEditSpellChecker::RichEditStreamOutCallback;
	
	PreSpellCheck();
    ::SendMessage(hwndEditCtrl, EM_STREAMOUT, (CP_UTF8 << 16) | SF_USECODEPAGE | SF_TEXT, reinterpret_cast<LPARAM>(&es));
	PostSpellCheck();
}

void CRichEditSpellChecker::PreSpellCheck()
{
	m_sWord.str("");
}

void CRichEditSpellChecker::PostSpellCheck()
{
	// Call this one last time in case we have a word waiting at the end of the stream.
	ProcessWord();
}

DWORD CRichEditSpellChecker::DoCallbackWork(LPBYTE pbBuff, LONG cb, LONG* pcb)
{
	istrstream iss(reinterpret_cast<char*>(pbBuff), cb);
	istreambuf_iterator<char> it(iss), itEnd;
	
	*pcb = 0;
	for ( ; it != itEnd; it++ )
	{
		if ( ((unsigned char)*it >= 0x80) || (!iscntrl(*it) && !isspace(*it) && !ispunct(*it)) )
			m_sWord << *it;
		else
			ProcessWord();

		(*pcb)++;
	}
	
	return 0;
}

void CRichEditSpellChecker::ProcessWord()
{
	string sWord(m_sWord.str());
	
	m_sWord.str("");
	
	if (!sWord.empty())
	{
		tsc_result tr = TSC_E_UNEXPECTED;
        TSC_CHECKWORD_DATA cw = {0};
		
		cw.cbSize = sizeof(cw);
		cw.nWordSize = sWord.size();
		cw.uTestWord.szWord8 = sWord.c_str();
		
		tr = m_pSession->CheckWord(&cw);
		
		//::OutputDebugStringA(sWord.c_str());
		
		if (TSC_FAILED(tr))
            ::MessageBox(NULL, L"Failure in CheckWord().", L"TolonSpellCheckLib", MB_OK | MB_ICONEXCLAMATION);
        else if (!cw.bOk)
        {
            ::OutputDebugStringA(sWord.c_str());
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
