#ifndef _TOLON_RICHEDIT_SPELL_CHECKER_H__
#define _TOLON_RICHEDIT_SPELL_CHECKER_H__

#include <windows.h>
#include <locale>
#include <sstream>

namespace TolonSpellCheck {

class CSession;

class CRichEditSpellChecker
{
    public:
        CRichEditSpellChecker(CSession* pSession);
        ~CRichEditSpellChecker();
    
    public:
        static DWORD CALLBACK RichEditStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
        
        void CheckSpelling(HWND hwndEditCtrl);
    
    protected:
        DWORD DoCallbackWork(LPBYTE pbBuff, LONG cb, LONG *pcb);
        void ProcessWord();
        void PreSpellCheck();
        void PostSpellCheck();

        std::locale m_locOld;
        std::locale m_loc;
        std::stringstream m_sWord;
        CSession* m_pSession;
};

} // end namespace

#endif
