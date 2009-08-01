#include "RichEditSpellChecker.h"
#include <assert.h>
#include <process.h>
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
    m_hWndRichEdit(NULL),
	m_pSession(pSession),
    m_nState(SpellCheckState_IDLE),
    m_dwCharsDone(0),
    m_dwCharsTotal(0),
    m_bStopFlag(false)
{
    ::InitializeCriticalSection(&m_cs);
}

CRichEditSpellChecker::~CRichEditSpellChecker()
{
    ::DeleteCriticalSection(&m_cs);
}

DWORD CRichEditSpellChecker::WT_RichEditStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CRichEditSpellChecker* pThis = reinterpret_cast<CRichEditSpellChecker*>(dwCookie);
	
	if (pThis)
		return pThis->WT_DoCallbackWork(pbBuff, cb, pcb);
	else
		return static_cast<DWORD>(-1);
}

void CRichEditSpellChecker::StartSpellCheck(HWND hwndEditCtrl)
{
    m_hWndRichEdit = hwndEditCtrl;
    m_bStopFlag = false;
    SetState(SpellCheckState_IDLE);

    unsigned int hThread = 0;
    hThread = _beginthreadex(NULL, 0, &WT_CheckSpelling, static_cast<void*>(this), CREATE_SUSPENDED, NULL);
    BOOL b = FALSE;
    b = ::SetThreadPriority((HANDLE)hThread, THREAD_PRIORITY_LOWEST);
    ::ResumeThread((HANDLE)hThread);
    ::CloseHandle((HANDLE)hThread);
}

void CRichEditSpellChecker::StopSpellCheck()
{
    SetStopFlag();
}

unsigned int CRichEditSpellChecker::WT_CheckSpelling(void* p)
{
    CRichEditSpellChecker* pThis = reinterpret_cast<CRichEditSpellChecker*>(p);

    if (pThis)
    {
	    EDITSTREAM es;
	    es.dwCookie = reinterpret_cast<DWORD>(pThis);
	    es.dwError = 0;
	    es.pfnCallback = &CRichEditSpellChecker::WT_RichEditStreamOutCallback;
    	
	    pThis->WT_PreSpellCheck();
        ::SendMessage(pThis->GetRichEditHwnd(), EM_STREAMOUT, SF_UNICODE | SF_TEXT, reinterpret_cast<LPARAM>(&es));
        pThis->WT_PostSpellCheck();
   }

    return 0;
}

void CRichEditSpellChecker::WT_PreSpellCheck()
{
    // Reset word container
	m_sWord.str(L"");

    // Find the total number of characters in the control.
    DWORD dwChars = 0;
    GETTEXTLENGTHEX etlex = {0};
    dwChars = ::SendMessage(GetRichEditHwnd(), EM_GETTEXTLENGTHEX, reinterpret_cast<WPARAM>(&etlex), NULL);
    SetTotalChars(dwChars);

    // Reset the char count
    SetCharsDone(0);
}

void CRichEditSpellChecker::WT_PostSpellCheck()
{
    if (IsStopFlagSet())
    {
        SetState(SpellCheckState_CANCELLED);
    }
    else
    {
	    // Call this one last time in case we have a word waiting at the end of the stream.
	    WT_ProcessWord();

        SetState(SpellCheckState_COMPLETE);
    }
}

DWORD CRichEditSpellChecker::WT_DoCallbackWork(LPBYTE pbBuff, LONG cb, LONG* pcb)
{
    DWORD dwResult = 0;
    const size_t nWideChars = cb / sizeof(std::wstring::value_type);

    std::wstring ws(reinterpret_cast<wchar_t*>(pbBuff), nWideChars);
    std::wstring::const_iterator it, itEnd;

    it = ws.begin();
    itEnd = ws.end();
	for ( ; it != itEnd; ++it )
	{
        if (IsUnicodeAlpha(*it))
			m_sWord << *it;
		else
			WT_ProcessWord();

        IncrementCharsDone();

        if (IsStopFlagSet())
        {
            dwResult = static_cast<DWORD>(-1);
            break;
        }
	}

    *pcb = nWideChars * sizeof(std::wstring::value_type);
	
	return dwResult;
}

void CRichEditSpellChecker::WT_ProcessWord()
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
        {
            assert(false);
        }
        else if (!cw.bOk)
        {
            //TODO: Something here!
            OutputDebugString(sWord.c_str());
            OutputDebugString(L"\r\n");
            Sleep(1000);
        }
	}
}

bool CRichEditSpellChecker::IsUnicodeAlpha(wchar_t wch)
{
    WORD nCharType = 0;
    ::GetStringTypeEx(LOCALE_USER_DEFAULT, CT_CTYPE1, &wch, 1, &nCharType);
    return (nCharType & C1_ALPHA) != 0;
}

void CRichEditSpellChecker::GetState(CRichEditSpellChecker::State& nState, DWORD& dwCharsDone, DWORD& dwCharsTotal)
{
    //::EnterCriticalSection(&m_cs);
    nState = m_nState;
    dwCharsDone = m_dwCharsDone;
    dwCharsTotal = m_dwCharsTotal;
    //::LeaveCriticalSection(&m_cs);
}

void CRichEditSpellChecker::SetState(CRichEditSpellChecker::State nState)
{
    //::EnterCriticalSection(&m_cs);
    m_nState = nState;
    //::LeaveCriticalSection(&m_cs);
}

void CRichEditSpellChecker::SetCharsDone(DWORD dwCharsDone)
{
    //::EnterCriticalSection(&m_cs);
    m_dwCharsDone = dwCharsDone;
    //::LeaveCriticalSection(&m_cs);
}

void CRichEditSpellChecker::SetTotalChars(DWORD dwTotalChars)
{
    //::EnterCriticalSection(&m_cs);
    m_dwCharsTotal = dwTotalChars;
    //::LeaveCriticalSection(&m_cs);
}

void CRichEditSpellChecker::SetStopFlag()
{
    m_bStopFlag = true;
}

bool CRichEditSpellChecker::IsStopFlagSet()
{
    return m_bStopFlag;
}

void CRichEditSpellChecker::IncrementCharsDone()
{
    m_dwCharsDone++;
}
