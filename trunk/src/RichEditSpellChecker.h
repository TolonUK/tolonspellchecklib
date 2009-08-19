#ifndef _TOLON_RICHEDIT_SPELL_CHECKER_H__
#define _TOLON_RICHEDIT_SPELL_CHECKER_H__

#include "TolonSpellCheck.h"
#include "utf8conv.h"
#include <windows.h>
#include <sstream>
#include <vector>
#include <map>

namespace TolonSpellCheck {

class CSession;

/*class CTolonSpellChecker
{
public:
    CTolonSpellChecker(CSession* pSession);
    virtual ~CTolonSpellChecker();

private:
        CSession* m_pSession;
};*/

class CRichEditSpellChecker
{
    public:
        enum State {
            SpellCheckState_INVALID,
            SpellCheckState_IDLE,
            SpellCheckState_WORKING,
            SpellCheckState_WAITING,
            SpellCheckState_COMPLETE,
            SpellCheckState_CANCELLED,
            SpellCheckState_ERROR };


    public:
        CRichEditSpellChecker(CSession* pSession);
        ~CRichEditSpellChecker();
    
    public:        
        void StartSpellCheck(HWND hwndEditCtrl);
        void StopSpellCheck();

        void GetState(CRichEditSpellChecker::State& nState, DWORD& dwCharsDone, DWORD& dwCharsTotal);

        TSC_CHECKWORD_DATA* GetCheckWordData();
        void ResumeSpellCheck();

        // Spell Check action when waiting for input
        void AddToDicAndResume();
        void IgnoreAndResume();
        void IgnoreAllAndResume();
        void ChangeAndResume(const wchar_t* psNewWord);
        void ChangeAllAndResume(const wchar_t* psNewWord);
        void SelectCurrentWord();
   
    private:
        //DWORD WT_DoCallbackWork(LPBYTE pbBuff, LONG cb, LONG *pcb);
        void WT_DoSpellCheckWork(const wchar_t* psData, size_t nChars);
        void WT_ProcessWord();
        void WT_PreSpellCheck();
        void WT_PostSpellCheck();
        void SetState(CRichEditSpellChecker::State nState);
        void SetCharsDone(DWORD dwCharsDone);
        void SetTotalChars(DWORD dwTotalChars);
        void SetStopFlag();
        bool IsStopFlagSet();
        void IncrementCharsDone();

        static unsigned int __stdcall WT_CheckSpelling(void* p);
        //static DWORD CALLBACK WT_RichEditStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
        static bool IsUnicodeAlpha(wchar_t wch, size_t n);

        HWND GetRichEditHwnd() const
        { return m_hWndRichEdit; }

        CRichEditSpellChecker::State GetState() const
        { return m_nState; }

        DWORD GetCharsDone() const
        { return m_dwCharsDone; }

        DWORD GetTotalChars() const
        { return m_dwCharsTotal; }

        bool InWaitingState() const
        { return GetState() == SpellCheckState_WAITING; }

        // These functions provide the response logic when waiting on an unrecognized word.
        void AddCurrentWordToCustomDic();
        void AddCurrentWordToIgnoreList();
        void ChangeCurrentWord(const wchar_t* psNewWord);
        void AddCurrentWordToChangeList(const wchar_t* psNewWord);
        int GetCurrentWordLength() const;

    private:
        HWND m_hWndRichEdit;
        std::wstringstream m_sWord;
        std::vector<wchar_t> m_vText;
        CSession* m_pSession;
        CUTF8Conv m_oConv;

    private:
        // Shared variables requiring synchronisation
        CRITICAL_SECTION m_cs;
        CRichEditSpellChecker::State m_nState;
        DWORD m_dwCharsDone;
        DWORD m_dwCharsTotal;
        bool m_bStopFlag;
        TSC_CHECKWORD_DATA m_cwd;
        HANDLE m_hResumeEvent;
        std::vector<std::wstring> m_vIgnoreList;
        std::map<std::wstring, std::wstring> m_xChangeList;
};

} // end namespace

#endif
