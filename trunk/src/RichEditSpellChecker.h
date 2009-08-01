#ifndef _TOLON_RICHEDIT_SPELL_CHECKER_H__
#define _TOLON_RICHEDIT_SPELL_CHECKER_H__

#include "utf8conv.h"
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

        static bool IsUnicodeAlpha(wchar_t wch);

    private:
        std::locale m_locOld;
        std::locale m_locDefault;
        std::locale m_loc;
        std::wstringstream m_sWord;
        CSession* m_pSession;
        CUTF8Conv m_oConv;
};

} // end namespace

#endif
