#ifndef _TOLON_UTF8CONV_H
#define _TOLON_UTF8CONV_H

#include <string>
#include <vector>

class CUTF8Conv
{
public:
    CUTF8Conv();

    bool utf8FromUnicode(const wchar_t* szUnicode, std::string& sOutput);

private:
    std::vector<char> m_vCharBuffer;
};

#endif
