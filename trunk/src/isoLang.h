#ifndef __TOLON_ISOLANG_H_INCLUDED
#define __TOLON_ISOLANG_H_INCLUDED

#include <string>

class CIsoLang
{
public:
    CIsoLang();

    void Parse(const char* szCode, std::wstring& sLanguage, std::wstring& sRegion);

private:
    void Parse_SplitCode(const char* szCode, std::string& sLangCode, std::string& sRegionCode);
    void Parse_GetLanguage(const char* szLangCode, std::wstring& sLanguage);
    void Parse_GetRegion(const char* szRegionCode, std::wstring& sRegion);
};

#endif //#__TOLON_ISOLANG_H_INCLUDED