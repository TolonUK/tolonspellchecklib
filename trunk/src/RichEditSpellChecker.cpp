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
#include <glib.h>
#include <algorithm>
#include <boost/mem_fn.hpp>

#pragma warning (disable: 4786)

using namespace std;
using namespace TolonSpellCheck;

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

    /*memset(&m_cwd, 0, sizeof(TSC_CHECKWORD_DATA));
	m_cwd.cbSize = sizeof(TSC_CHECKWORD_DATA);
    m_cwd.uResultString.szResults8 = new char[s_nResultBufLen];*/

    m_hResumeEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

CRichEditSpellChecker::~CRichEditSpellChecker()
{
    ::DeleteCriticalSection(&m_cs);

    //delete [] m_cwd.uResultString.szResults8;

    ::CloseHandle(m_hResumeEvent);
}

void CRichEditSpellChecker::StartSpellCheck(HWND hwndEditCtrl)
{
    m_hWndRichEdit = hwndEditCtrl;
    m_bStopFlag = false;
    SetState(SpellCheckState_IDLE);

    unsigned int hThread = 0;
    hThread = _beginthreadex(NULL, 0, &WT_CheckSpelling, static_cast<void*>(this), CREATE_SUSPENDED, NULL);
    ::SetThreadPriority((HANDLE)hThread, THREAD_PRIORITY_LOWEST);
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

void CRichEditSpellChecker::WT_DoSpellCheckWork(const wchar_t* psData, size_t nChars)
{
    size_t nCharsThisWord = 0;
    size_t nDone = 0;
    for ( ; nDone < nChars; ++nDone )
    {
        if (*psData == NULL)
            break;
        else if (IsUnicodeAlpha(*psData, nCharsThisWord))
        {
			m_sWord << *psData;
            nCharsThisWord++;
        }
		else
        {
			WT_ProcessWord();
            nCharsThisWord = 0;
        }

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
    bool bDoProcessing = true;
	wstring sWord(m_sWord.str());

    // Check to see if the word is on the ignore or change list.
    if (std::find(m_vIgnoreList.begin(), m_vIgnoreList.end(), sWord) != m_vIgnoreList.end())
    {
        bDoProcessing = false;
    }
    else
    {
        std::map<std::wstring, std::wstring>::iterator it;
        it = m_xChangeList.find(sWord);
        if (it != m_xChangeList.end())
        {
            ChangeCurrentWord(it->second.c_str());
            bDoProcessing = false;
        }
    }

    // Now check the word if required
    if (bDoProcessing)
    {
        if (!sWord.empty())
        {
            m_cwd.TestWord(sWord.c_str());

            // Call m_pSession->CheckWord using m_cwd as the parameter.
            m_cwd.Call(std::bind1st(std::mem_fun(&CSession::CheckWord), m_pSession));
            
            if (!m_cwd.WordWasOk())
            {
                SetState(SpellCheckState_WAITING);
                ::WaitForSingleObject(m_hResumeEvent, INFINITE);
            }
        }
    }

    m_sWord.str(L"");
}

bool CRichEditSpellChecker::IsUnicodeAlpha(wchar_t wch, size_t n)
{
    //::GetStringTypeEx(LOCALE_USER_DEFAULT, CT_CTYPE3, &wch, 1, &nCharType);
    //return (nCharType & (C3_ALPHA | C3_LEXICAL)) != 0;

    GUnicodeType type;
	type = g_unichar_type(wch);

	switch (type) {
	case G_UNICODE_MODIFIER_LETTER:
	case G_UNICODE_LOWERCASE_LETTER:
	case G_UNICODE_TITLECASE_LETTER:
	case G_UNICODE_UPPERCASE_LETTER:
	case G_UNICODE_OTHER_LETTER:
	case G_UNICODE_COMBINING_MARK:
	case G_UNICODE_ENCLOSING_MARK:
	case G_UNICODE_NON_SPACING_MARK:
	case G_UNICODE_DECIMAL_NUMBER:
	case G_UNICODE_LETTER_NUMBER:
	case G_UNICODE_OTHER_NUMBER:
	case G_UNICODE_CONNECT_PUNCTUATION:
                return true;     // Enchant 1.3.0 defines word chars like this.

	case G_UNICODE_CONTROL:
	case G_UNICODE_FORMAT:
	case G_UNICODE_UNASSIGNED:
	case G_UNICODE_PRIVATE_USE:
	case G_UNICODE_SURROGATE:
	case G_UNICODE_DASH_PUNCTUATION:
	case G_UNICODE_CLOSE_PUNCTUATION:
	case G_UNICODE_FINAL_PUNCTUATION:
	case G_UNICODE_INITIAL_PUNCTUATION:
	case G_UNICODE_OTHER_PUNCTUATION:
	case G_UNICODE_OPEN_PUNCTUATION:
	case G_UNICODE_CURRENCY_SYMBOL:
	case G_UNICODE_MODIFIER_SYMBOL:
	case G_UNICODE_MATH_SYMBOL:
	case G_UNICODE_OTHER_SYMBOL:
	case G_UNICODE_LINE_SEPARATOR:
	case G_UNICODE_PARAGRAPH_SEPARATOR:
	case G_UNICODE_SPACE_SEPARATOR:
	default:
		if ((n > 0) && (wch == L'\'')) {
		        return true;  // Char ' is accepted only within a word.
		}
		else if ((n > 0) && (type == G_UNICODE_DASH_PUNCTUATION)) {
			return true; // hyphens only accepted within a word.
		}

		return false;
	}
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

CCheckWordData& CRichEditSpellChecker::GetCheckWordData()
{
    return m_cwd;
}

void CRichEditSpellChecker::ResumeSpellCheck()
{
    assert(InWaitingState());

    if (InWaitingState())
    {
        SetState(SpellCheckState_WORKING);
        ::SetEvent(m_hResumeEvent);
    }
}

void CRichEditSpellChecker::AddToDicAndResume()
{
    assert(InWaitingState());

    if (InWaitingState())
    {
        AddCurrentWordToCustomDic();
        ResumeSpellCheck();
    }
}

void CRichEditSpellChecker::IgnoreAndResume()
{
    assert(InWaitingState());

    if (InWaitingState())
    {
        ResumeSpellCheck();
    }
}

void CRichEditSpellChecker::IgnoreAllAndResume()
{
    assert(InWaitingState());

    if (InWaitingState())
    {
        AddCurrentWordToIgnoreList();
        ResumeSpellCheck();
    }
}

void CRichEditSpellChecker::ChangeAndResume(const wchar_t* psNewWord)
{
    assert(InWaitingState());

    if (InWaitingState())
    {
        ChangeCurrentWord(psNewWord);
        ResumeSpellCheck();
    }
}

void CRichEditSpellChecker::ChangeAllAndResume(const wchar_t* psNewWord)
{
    assert(InWaitingState());

    if (InWaitingState())
    {
        AddCurrentWordToChangeList(psNewWord);
        ChangeCurrentWord(psNewWord);
        ResumeSpellCheck();
    }
}

void CRichEditSpellChecker::AddCurrentWordToCustomDic()
{
    assert(InWaitingState());

    if (InWaitingState())
    {
		if (m_pSession)
		{
			CCustomDicData cdd;
			cdd.Word(m_sWord.str());
			cdd.Action(CUSTOMDICACTION_ADDWORD);

            // Call m_pSession->CustomDic using cdd as the parameter.
            cdd.Call(std::bind1st(std::mem_fun(&CSession::CustomDic), m_pSession));
		}
    }
}

void CRichEditSpellChecker::AddCurrentWordToIgnoreList()
{
    assert(InWaitingState());

    if (InWaitingState())
    {
        std::wstring sWord(m_sWord.str());
        m_vIgnoreList.push_back(sWord);
    }
}

void CRichEditSpellChecker::ChangeCurrentWord(const wchar_t* psNewWord)
{
    assert(psNewWord);

    if (psNewWord)
    {
        HWND hwnd = GetRichEditHwnd();

        if (hwnd)
        {
            const int nCurrentLength = GetCurrentWordLength();
            const int nNewLength = wcslen(psNewWord);
            SelectCurrentWord();
            ::SendMessage(hwnd, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(psNewWord));
            SetCharsDone(GetCharsDone() + (nNewLength - nCurrentLength));
            SetTotalChars(GetTotalChars() + (nNewLength - nCurrentLength));
        }
    }
}

void CRichEditSpellChecker::AddCurrentWordToChangeList(const wchar_t* psNewWord)
{
    assert(InWaitingState());

    if (InWaitingState())
    {
        std::wstring sOldWord(m_sWord.str());
        std::wstring sNewWord(psNewWord);
        m_xChangeList.insert(std::make_pair(sOldWord, sNewWord));
    }
}

void CRichEditSpellChecker::SelectCurrentWord()
{
    HWND hwnd = GetRichEditHwnd();
    if (hwnd)
    {
        DWORD dwCharsDone = GetCharsDone();
        std::wstring sWord(m_sWord.str());

        ::SendMessage(hwnd, EM_SETSEL, dwCharsDone - sWord.size(), dwCharsDone);
        ::SendMessage(hwnd, EM_SCROLLCARET, 0, 0);
    }
}

int CRichEditSpellChecker::GetCurrentWordLength() const
{
    return m_sWord.str().size();
}
