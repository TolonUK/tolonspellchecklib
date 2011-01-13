#include "utf8conv.h"
#include <algorithm>
#include <iterator>
#include <string>
#include <windows.h>

using namespace std;

CUTF8Conv::CUTF8Conv()
{
}

bool CUTF8Conv::utf8FromUnicode(const wchar_t* szUnicode, std::string& sOutput)
{
    sOutput.clear();
    
    if (szUnicode)
    {
        const std::wstring wsSrc(szUnicode);
        int nResult = 0;
        const int nMinCharBufSize = (wsSrc.size() + 1) * sizeof(std::wstring::value_type);
        
        if (m_vCharBuffer.size() < nMinCharBufSize)
        {
            m_vCharBuffer.resize(nMinCharBufSize);
        }

        nResult = ::WideCharToMultiByte(
            CP_UTF8,
            0,
            wsSrc.c_str(),
            -1,
            &(*m_vCharBuffer.begin()),
            m_vCharBuffer.size(),
            NULL,
            NULL );

        if ( (nResult != 0) && (nResult <= m_vCharBuffer.size()) )
        {
            std::vector<char>::iterator itEnd = std::find(m_vCharBuffer.begin(), m_vCharBuffer.end(), '\0');
            std::copy(m_vCharBuffer.begin(), itEnd, std::back_inserter(sOutput));
        }
    }
    
    return !sOutput.empty();
}