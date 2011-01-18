#include "utf8conv.h"
#include <windows.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <cassert>

using namespace std;

// reads a UTF8 storage string into a runtime wchar_t string.
bool string_from_utf8(std::wstring& dest, const char* src, size_t srclen)
{
    // error conditions
    if (src == NULL)
        return false;

    //trivial case
    if (srclen == 0)
    {
        dest.clear();
        return true;
    }
    else if (srclen == static_cast<size_t>(-1))
    {
        srclen = strlen(src);
    }

    bool bResult = false;
    char* safesrc = new char[srclen + 1];
    wchar_t* wszBuf = new wchar_t[srclen];

    memcpy(safesrc, src, srclen);
    safesrc[srclen] = 0;

    memset(wszBuf, 0, srclen * sizeof(wchar_t));

    const int n = ::MultiByteToWideChar(CP_UTF8, 0, safesrc, -1, wszBuf, srclen);
    assert(n);

    if (n)
    {
        dest = wszBuf;
        bResult = true;
    }

    return bResult;
}

bool utf8_from_string(char* dest, const size_t destlen, const std::wstring& src)
{
    // error conditions
    if ((dest == NULL) || (destlen == 0))
        return false;

    // trivial case
    if (src.empty())
    {
        *dest = 0;
        return true;
    }

    bool bResult = false;

    memset(dest, 0, destlen);

    const int n = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, dest, destlen, NULL, NULL);
    assert(n);

    bResult = (n > 0);

    return bResult;
}

CUTF8Conv::CUTF8Conv()
{
}

bool CUTF8Conv::utf8FromUnicode(const wchar_t* szUnicode, std::string& sOutput)
{
    sOutput.clear();
    
    if (szUnicode)
    {
        const std::wstring wsSrc(szUnicode);
        size_t nResult = 0;
        const size_t nMinCharBufSize = (wsSrc.size() + 1) * sizeof(std::wstring::value_type);
        
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