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

static size_t s_nResultBufLen = 1024;

CRichEditSpellChecker::CRichEditSpellChecker(CSession* pSession) : 
    m_hWndRichEdit(NULL),
	m_pSession(pSession),
    m_nState(SpellCheckState_IDLE),
    m_dwCharsDone(0),
    m_dwCharsTotal(0),
    m_bStopFlag(false),
    m_hResumeEvent(NULL)
{
    ::InitializeCriticalSection(&m_cs);

    memset(&m_cwd, 0, sizeof(TSC_CHECKWORD_DATA));
	m_cwd.cbSize = sizeof(TSC_CHECKWORD_DATA);
    m_cwd.uResultString.szResults8 = new char[s_nResultBufLen];

    m_hResumeEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

CRichEditSpellChecker::~CRichEditSpellChecker()
{
    ::DeleteCriticalSection(&m_cs);

    delete [] m_cwd.uResultString.szResults8;

    ::CloseHandle(m_hResumeEvent);
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
    ::SetEvent(m_hResumeEvent);
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

        LRESULT lr = 0;
        GETTEXTLENGTHEX gtlex = {0};
        gtlex.codepage = 1200; //CP_UNICODE
        gtlex.flags = GTL_NUMCHARS | GTL_CLOSE | GTL_USECRLF;
        lr = ::SendMessage(pThis->GetRichEditHwnd(), EM_GETTEXTLENGTHEX, reinterpret_cast<WPARAM>(&gtlex), NULL);

        if (lr > 0)
        {
            GETTEXTEX gtex = {0};
            pThis->m_vText.resize(lr + 1);
            gtex.cb = pThis->m_vText.size() * sizeof(wchar_t);
            gtex.flags = GT_DEFAULT;
            gtex.codepage = 1200; //CP_UNICODE
            lr = ::SendMessage(pThis->GetRichEditHwnd(), EM_GETTEXTEX, reinterpret_cast<WPARAM>(&gtex), reinterpret_cast<LPARAM>(&(*pThis->m_vText.begin())));
            pThis->WT_DoSpellCheckWork(&(*pThis->m_vText.begin()), lr);
        }

        pThis->WT_PostSpellCheck();
   }

    return 0;
}

void CRichEditSpellChecker::WT_PreSpellCheck()
{
    SetState(SpellCheckState_WORKING);

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

void CRichEditSpellChecker::WT_DoSpellCheckWork(const wchar_t* psData, size_t nChars)
{
    size_t nDone = 0;
    for ( ; nDone < nChars; ++nDone )
    {
        if (*psData == NULL)
            break;
        else if (IsUnicodeAlpha(*psData))
			m_sWord << *psData;
		else
			WT_ProcessWord();

        IncrementCharsDone();

        if (IsStopFlagSet())
        {
            break;
        }
        
        ++psData;
    }
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
		
		m_cwd.nWordSize = sUtf8Word.size();
		m_cwd.uTestWord.szWord8 = sUtf8Word.c_str();
        m_cwd.nResultStringSize = s_nResultBufLen;
		
		tr = m_pSession->CheckWord(&m_cwd);
		
		if (TSC_FAILED(tr))
        {
            assert(false);
        }
        else if (!m_cwd.bOk)
        {
            SetState(SpellCheckState_WAITING);
            ::WaitForSingleObject(m_hResumeEvent, INFINITE);
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

TSC_CHECKWORD_DATA* CRichEditSpellChecker::GetCheckWordData()
{
    TSC_CHECKWORD_DATA* p = NULL;

    if (InWaitingState())
        p = &m_cwd;

    return p;
}

void CRichEditSpellChecker::ResumeSpellCheck()
{
    if (InWaitingState())
    {
        SetState(SpellCheckState_WORKING);
        ::SetEvent(m_hResumeEvent);
    }
}

void CRichEditSpellChecker::AddToDicAndResume()
{
    if (InWaitingState())
    {
        AddCurrentWordToCustomDic();
        ResumeSpellCheck();
    }
}

void CRichEditSpellChecker::IgnoreAndResume()
{
    if (InWaitingState())
    {
        ResumeSpellCheck();
    }
}

void CRichEditSpellChecker::IgnoreAllAndResume()
{
    if (InWaitingState())
    {
        AddCurrentWordToIgnoreList();
        ResumeSpellCheck();
    }
}

void CRichEditSpellChecker::ChangeAndResume(const wchar_t* psNewWord)
{
    if (InWaitingState())
    {
        ChangeCurrentWord(psNewWord);
        ResumeSpellCheck();
    }
}

void CRichEditSpellChecker::ChangeAllAndResume(const wchar_t* psNewWord)
{
    if (InWaitingState())
    {
        AddCurrentWordToChangeList(psNewWord);
        ChangeCurrentWord(psNewWord);
        ResumeSpellCheck();
    }
}

void CRichEditSpellChecker::AddCurrentWordToCustomDic()
{
}

void CRichEditSpellChecker::AddCurrentWordToIgnoreList()
{
}

void CRichEditSpellChecker::ChangeCurrentWord(const wchar_t* psNewWord)
{
}

void CRichEditSpellChecker::AddCurrentWordToChangeList(const wchar_t* psNewWord)
{
}
