#ifndef __TOLON_ISOLANG_H_INCLUDED
#define __TOLON_ISOLANG_H_INCLUDED

class CIsoLang
{
public:
    CIsoLang();

    void Parse(const char* szCode, std::wstring& sLanguage, std::wstring& sRegion);
};

#endif //#__TOLON_ISOLANG_H_INCLUDED