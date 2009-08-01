#ifndef _TOLON_RICHEDIT_SPELL_CHECKER_H__
#define _TOLON_RICHEDIT_SPELL_CHECKER_H__

#include "utf8conv.h"
#include <windows.h>
#include <sstream>

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
        CRichEditSpellChecker(CSession* pSession);
        ~CRichEditSpellChecker();
    
    public:        
        void CheckSpelling(HWND hwndEditCtrl);
    
    private:
        DWORD DoCallbackWork(LPBYTE pbBuff, LONG cb, LONG *pcb);
        void ProcessWord();
        void PreSpellCheck();
        void PostSpellCheck();

        static DWORD CALLBACK RichEditStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
        static bool IsUnicodeAlpha(wchar_t wch);

    private:
        std::wstringstream m_sWord;
        CSession* m_pSession;
        CUTF8Conv m_oConv;
};

} // end namespace

#endif
