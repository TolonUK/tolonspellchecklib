#ifndef _TOLON_UTF8CONV_H
#define _TOLON_UTF8CONV_H

#include <string>
#include <vector>

// reads a UTF8 storage string into a runtime wchar_t string.
bool string_from_utf8(std::wstring& dest, const char* src, size_t srclen);
bool utf8_from_string(char* dest, const size_t destlen, const std::wstring& src);

class CUTF8Conv
{
public:
    CUTF8Conv();

    bool utf8FromUnicode(const wchar_t* szUnicode, std::string& sOutput);
    bool unicodeFromUtf8(const char* szUtf8, std::wstring& sOutput);

private:
    std::vector<char> m_vCharBuffer;
    std::vector<wchar_t> m_vWideBuffer;
};

#endif
