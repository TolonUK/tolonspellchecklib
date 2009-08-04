#ifndef __TOLON_ISOLANG_H_INCLUDED
#define __TOLON_ISOLANG_H_INCLUDED

#include <string>

class CIsoLang
{
public:
    CIsoLang();

    void Parse(const char* szCode, std::string& sLanguage, std::string& sRegion);

private:
    void Parse_SplitCode(const char* szCode, std::string& sLangCode, std::string& sRegionCode);
    void Parse_GetLanguage(const char* szLangCode, std::string& sLanguage);
    void Parse_GetRegion(const char* szRegionCode, std::string& sRegion);
};

#endif //#__TOLON_ISOLANG_H_INCLUDED