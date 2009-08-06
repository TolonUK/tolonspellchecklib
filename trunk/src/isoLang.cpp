#include "isoLang.h"
#include <algorithm>
#include <functional>
#include <locale>

// http://www.loc.gov/standards/iso639-2/php/English_list.php
// http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes
// http://www.evertype.com/emono/
// http://ftp.gnu.org/gnu/aspell/dict/0index.html

struct ISO3166Region {
    const char* Alpha2Code; // ASCII
    const char* EnglishName;
    const char* NativeName; // UTF8
};

const ISO3166Region g_regions[] = {
    { "de", "Germany", "Germany" },
    { "fr", "France", "France" },
    { "gb", "United Kingdom", "United Kingdom" },
    { "ph", "Philippines", "Philippines" },
    { "ru", "Russia", "Russia" },
    { "us", "United States", "United States" }
};

const size_t g_regions_size = sizeof(g_regions) / sizeof(g_regions[0]);

struct ISO639Language {
    const char* Level1Code; // ASCII
    const char* Level2CodeT; // ASCII
    const char* Level2CodeB; // ASCII
    const char* Level3Code; // ASCII
    const char* EnglishName;
    const char* NativeName; // UTF8
};

// Need right-to-left indicator?
const ISO639Language g_languages[] = {
    { "af", "afr", NULL, "afr", "Afrikaans", "Afrikaans" },
    { "ar", "ara", NULL, "ara", "Arabic", "العربية" }, // 30 variations?
    { "bo", "bod", "tib", "bod", "Tibetan", "བོད་ཡིག" },
    { "ca", "cat", NULL, "cat", "Catalan", "Català" },
    { "cs", "ces", "cze", "ces", "Czech", "česky" },
    { "da", "dan", NULL, "dan", "Danish", "dansk" },
    { "de", "deu", "ger", "deu", "German", "Deutsch" },
    { "en", "eng", NULL, "eng", "English", "English" },
    { "eo", "epo", NULL, "epo", "Esperanto", "Esperanto" },
    { "es", "spa", NULL, "spa", "Spanish", "español" },
    { "fi", "fin", NULL, "fin", "Finnish", "suomi" },
    { "fr", "fra", "fre", "fra", "French", "Français" },
    { "hu", "hun", NULL, "hun", "Hungarian", "Magyar" },
    { "id", "ind", NULL, "ind", "Indonesian", "Bahasa Indonesia" },
    { "it", "ita", NULL, "ita", "Italian", "Italiano" },
    { "ja", "jpn", NULL, "jpn", "Japanese", "日本語 (にほんご／にっぽんご)" },
    { "ko", "kor", NULL, "kor", "Korean", "한국어 (韓國語), 조선말 (朝鮮語)" },
    { "nl", "nld", "dut", "nld", "Dutch", "Nederlands" },
    { "no", "nor", NULL, "nor", "Norwegian", "Norsk" },
    /*pl pol - pol Polish polski 
    pt por - por Portuguese Português 
    ro ron rum ron Romanian română*/
    { "ru", "rus", NULL, "rus", "Russian", "русский язык" }
    /*sk slk slo slk Slovak slovenčina 
    sv swe - swe Swedish svenska 
    tr tur - tur Turkish Türkçe 
    uk ukr - ukr Ukrainian Українська 
    vo vol - vol Volapük Volapük
    zh zho chi zho + 13 Chinese 中文 (Zhōngwén), 汉语, 漢語 */
};	

const size_t g_languages_size = sizeof(g_languages) / sizeof(g_languages[0]);

struct MacroLanguageMember {
    const char* Code;
    const char* ParentCode;
    const char* EnglishName;
    const char* NativeName;
};

const MacroLanguageMember g_language_variants[] = {
    { "aao", "ara", "Algerian Saharan Arabic", "" },
    { "abh", "ara", "Tajiki Arabic", "" },
    { "abv", "ara", "Baharna Arabic", "" },
    { "acm", "ara", "Mesopotamian Arabic", "" },
    { "acq", "ara", "Ta'izzi-Adeni Arabic", "" },
    { "acw", "ara", "Hijazi Arabic", "" },
    { "acx", "ara", "Omani Arabic", "" },
    { "acy", "ara", "Cypriot Arabic", "" },
    { "adf", "ara", "Dhofari Arabic", "" },
    { "aeb", "ara", "Tunisian Arabic", "" },
    { "aec", "ara", "Saidi Arabic", "" },
    { "afb", "ara", "Gulf Arabic", "" },
    { "ajp", "ara", "South Levantine Arabic", "" }
/*apc — North Levantine Arabic 
apd — Sudanese Arabic 
arb — Standard Arabic 
arq — Algerian Arabic 
ars — Najdi Arabic 
ary — Moroccan Arabic 
arz — Egyptian Arabic 
auz — Uzbeki Arabic 
avl — Eastern Egyptian Bedawi Arabic 
ayh — Hadrami Arabic 
ayl — Libyan Arabic 
ayn — Sanaani Arabic 
ayp — North Mesopotamian Arabic 
bbz — Babalia Creole Arabic 
pga — Sudanese Creole Arabic 
shu — Chadian Arabic 
ssh — Shihhi Arabic*/
};

CIsoLang::CIsoLang()
{
    
}

void CIsoLang::Parse(const char* szCode, std::string& sLanguage, std::string& sRegion)
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
            sLangCode.push_back(*it);
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
            sRegionCode.push_back(*it);
        else
            break;
    }     
}

void CIsoLang::Parse_GetLanguage(const char* szLangCode, std::string& sLanguage)
{
    std::locale locDefault;
    std::string sCode(szLangCode);
    const int nCodeSize = sCode.size();
    
    sLanguage.clear();
    
    //std::transform(sCode.begin(), sCode.end(), sCode.begin(), std::bind2nd(std::ptr_fun(std::tolower<char>), locDefault));
    for ( std::string::iterator it = sCode.begin();
          it != sCode.end();
          ++it )
    {
        *it = std::tolower(*it, locDefault);
    }
    
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

void CIsoLang::Parse_GetRegion(const char* szRegionCode, std::string& sRegion)
{
    std::locale locDefault;
    std::string sCode(szRegionCode);
    const int nCodeSize = sCode.size();
    
    sRegion.clear();
    
    //std::transform(sCode.begin(), sCode.end(), sCode.begin(), std::bind2nd(std::tolower<char>, locDefault));
    for ( std::string::iterator it = sCode.begin();
          it != sCode.end();
          ++it )
    {
        *it = std::tolower(*it, locDefault);
    }
    
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
