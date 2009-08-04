#include "isoLang.h"
#include <algorithm>
#include <locale>

// http://www.loc.gov/standards/iso639-2/php/English_list.php
// http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes
// http://www.evertype.com/emono/

struct ISO3166Region {
    char* Alpha2Code; // ASCII
    wchar_t* EnglishName;
    char* NativeName; // UTF8
};

const ISO3166Region g_regions[] = {
    { "de", L"Germany", "Germany" },
    { "fr", L"France", "France" },
    { "gb", L"United Kingdom", "United Kingdom" },
    { "ru", L"Russia", "Russia" },
    { "us", L"United States", "United States" }
};

const size_t g_regions_size = sizeof(g_regions) / sizeof(g_regions[0]);

struct ISO639Language {
    char* Level1Code; // ASCII
    char* Level2CodeT; // ASCII
    char* Level2CodeB; // ASCII
    char* Level3Code; // ASCII
    wchar_t* EnglishName;
    char* NativeName; // UTF8
};

// Need right-to-left indicator?
const ISO639Language g_languages[] = {
    { "af", "afr", NULL, "afr", L"Afrikaans", "Afrikaans" },
    { "ar", "ara", NULL, "ara", L"Arabic", "العربية" }, // 30 variations?
    { "bo", "bod", "tib", "bod", L"Tibetan", "བོད་ཡིག" },
    { "ca", "cat", NULL, "cat", L"Catalan", "Català" },
    { "cs", "ces", "cze", "ces", L"Czech", "česky" },
    { "da", "dan", NULL, "dan", L"Danish", "dansk" },
    { "de", "deu", "ger", "deu", L"German", "Deutsch" },
    { "en", "eng", NULL, "eng", L"English", "English" },
    { "eo", "epo", NULL, "epo", L"Esperanto", "Esperanto" },
    { "es", "spa", NULL, "spa", L"Spanish", "español" },
    { "fi", "fin", NULL, "fin", L"Finnish", "suomi" },
    { "fr", "fra", "fre", "fra", L"French", "Français" },
    { "hu", "hun", NULL, "hun", L"Hungarian", "Magyar" },
    { "id", "ind", NULL, "ind", L"Indonesian", "Bahasa Indonesia" },
    { "it", "ita", NULL, "ita", L"Italian", "Italiano" },
    { "ja", "jpn", NULL, "jpn", L"Japanese", "日本語 (にほんご／にっぽんご)" },
    { "ko", "kor", NULL, "kor", L"Korean", "한국어 (韓國語), 조선말 (朝鮮語)" }//,
    /*nl nld dut nld Dutch Nederlands 
    no nor - nor + 2 Norwegian Norsk 
    pl pol - pol Polish polski 
    pt por - por Portuguese Português 
    ro ron rum ron Romanian română 
    ru rus - rus Russian русский язык 
    sk slk slo slk Slovak slovenčina 
    sv swe - swe Swedish svenska 
    tr tur - tur Turkish Türkçe 
    uk ukr - ukr Ukrainian Українська 
    vo vol - vol Volapük Volapük
    zh zho chi zho + 13 Chinese 中文 (Zhōngwén), 汉语, 漢語 */
};	

const size_t g_languages_size = sizeof(g_languages) / sizeof(g_languages[0]);

CIsoLang::CIsoLang()
{
    
}

void CIsoLang::Parse(const char* szCode, std::wstring& sLanguage, std::wstring& sRegion)
{
    std::string sLangCode;
    std::string sRegionCode;
    
    sLanguage.clear();
    sRegion.clear();
    
    Parse_SplitCode(szCode, sLangCode, sRegionCode);
    if (!sLangCode.empty())
    {
        Parse_GetLanguage(sLangCode.c_str(), sLanguage);
    }
    if (!sRegionCode.empty())
    {
        Parse_GetRegion(sRegionCode.c_str(), sRegion); 
    }
}

void CIsoLang::Parse_SplitCode(const char* szCode, std::string& sLangCode, std::string& sRegionCode)
{
    std::string sCode(szCode);
    
    sLangCode.clear();
    sRegionCode.clear();
    
    std::string::iterator it = sCode.begin();
    // Get chars for lang code
    for ( ; it != sCode.end(); ++it )
    {
        if (isalpha(*it))
            sLang.push_back(*it);
        else
            break;
    }
    // Skip non-alpha chars
    for ( ; it != sCode.end(); ++it )
    {
        if (isalpha(*it))
            break;
    }
    // Get chars for region code
    for ( ; it != sCode.end(); ++it )
    {
        if (isalpha(*it))
            sRegion.push_back(*it);
        else
            break;
    }     
}

void CIsoLang::Parse_GetLanguage(const char* szLangCode, std::wstring& sLanguage)
{
    std::locale locDefault;
    std::string sCode(szLangCode);
    const int nCodeSize = sCode.size();
    
    sLanguage.clear();
    
    std::transform(sCode.begin(), sCode.end(), sCode.begin(), std::bind2nd(std::tolower<char>, locDefault));
    
    const char* psz = sCode.c_str();
    
    if (nCodeSize == 2)
    {
        for (size_t i = 0; i < g_languages_size; ++i)
        {
            if (memcmp(psz, g_languages[i].Level1Code, nCodeSize) == 0)
            {
                sLanguage.assign(g_languages[i].EnglishName);
                break;
            }
        }
    }
    else if (nCodeSize == 3)
    {
        bool bFound = false;
        
        for (size_t i = 0; i < g_languages_size; ++i)
        {
            bFound = memcmp(psz, g_languages[i].Level2CodeT, nCodeSize) == 0;
            if (!bFound && (g_languages[i].Level2CodeB != NULL))
                bFound = memcmp(psz, g_languages[i].Level2CodeB, nCodeSize) == 0;
            
            if (bFound)
            {
                sLanguage.assign(g_languages[i].EnglishName);
                break;
            }
        }
    }
}

void CIsoLang::Parse_GetRegion(const char* szRegionCode, std::wstring& sRegion)
{
    std::locale locDefault;
    std::string sCode(szRegionCode);
    const int nCodeSize = sCode.size();
    
    sRegion.clear();
    
    std::transform(sCode.begin(), sCode.end(), sCode.begin(), std::bind2nd(std::tolower<char>, locDefault));
    
    const char* psz = sCode.c_str();
    
    if (nCodeSize == 2)
    {
        for (size_t i = 0; i < g_regions_size; ++i)
        {
            if (memcmp(psz, g_regions[i].Alpha2Code, nCodeSize) == 0)
            {
                sRegion.assign(g_regions[i].EnglishName);
                break;
            }
        }
    }
}
