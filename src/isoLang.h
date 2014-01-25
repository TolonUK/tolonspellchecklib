#ifndef __TOLON_ISOLANG_H_INCLUDED
#define __TOLON_ISOLANG_H_INCLUDED

#include <string>

class CIsoLang
{
private:
    CIsoLang();

public:
    static void Parse(const char* szCode, std::string& sLanguage, std::string& sRegion);

    static int Compare(const char* szLhs, const char* szRhs);
    static int Compare(const wchar_t* szLhs, const wchar_t* szRhs);

    static void GetDisplayName(const char* szCode, std::string& sDisplay);
    static void GetDisplayName(const wchar_t* szCode, std::wstring& sDisplay);

private:
    static void Parse_SplitCode(const char* szCode, std::string& sLangCode, std::string& sRegionCode);
    static void Parse_GetLanguage(const char* szLangCode, std::string& sLanguage);
    static void Parse_GetRegion(const char* szRegionCode, std::string& sRegion);

    static void make_lower(std::string& s);
};

#endif //#__TOLON_ISOLANG_H_INCLUDED