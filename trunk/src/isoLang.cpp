#include "isoLang.h"
#include "utf8conv.h"
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
    { "am", "Armenia", "Armenia" },
    { "at", "Austria", "Austria" },
    { "au", "Australia", "Australia" },
    { "bg", "Bulgaria", "Bulgaria" },
    { "bo", "Bolivia", "Bolivia" },
    { "br", "Brazil", "Brazil" },
    { "ca", "Canada", "Canada" },
    { "ch", "Switzerland", "Switzerland" },
    { "cz", "Czech Republic", "Czech Republic" },
    { "de", "Germany", "Germany" },
    { "dk", "Denmark", "Denmark" },
    { "ee", "Estonia", "Estonia" },
    { "eg", "Egypt", "Egypt" },
    { "es", "Spain", "Spain" },
    { "fj", "Fiji", "Fiji" },
    { "fo", "Faroe Islands" },
    { "fr", "France", "France" },
    { "gb", "United Kingdom", "United Kingdom" },
    { "gr", "Greece", "Greece" },
    { "hu", "Hungary", "Hungary" },
    { "hr", "Croatia", "Croatia" },
    { "id", "Indonesia", "Indonesia" },
    { "ie", "Ireland", "Ireland" },
    { "il", "Israel", "Israel" },
    { "in", "India", "India" },
    { "ir", "Iran", "Iran" },
    { "is", "Iceland", "Iceland" },
    { "it", "Italy", "Italy" },
    { "ke", "Kenya", "Kenya" },
    { "kh", "Cambodia", "Cambodia" },
    { "lt", "Lithuania", "Lithuania" },
    { "lv", "Latvia", "Latvia" },
    { "mg", "Madagascar", "Madagascar" },
    { "mn", "Mongolia", "Mongolia"},
    { "mw", "Malawi", "Malawi" },
    { "mx", "Mexico", "Mexico" },
    { "my", "Malaysia", "Malaysia" },
    { "nl", "Netherlands", "Netherlands" },
    { "no", "Norway", "Norway" },
    { "np", "Nepal", "Nepal" },
    { "nz", "New Zeland", "New Zeland" },
    { "ph", "Philippines", "Philippines" },
    { "pl", "Poland", "Poland" },
    { "pt", "Portugal", "Portugal" },
    { "ro", "Romania", "Romania" },
    { "ru", "Russia", "Russia" },
    { "rw", "Rwanda", "Rwanda" },
    { "se", "Sweden", "Sweden" },
    { "si", "Slovenia", "Slovenia" },
    { "sk", "Slovakia", "Slovakia" },
    { "th", "Thailand", "Thailand" },
    { "tr", "Turkey", "Turkey" },
    { "tz", "Tanzania", "Tanzania" },
    { "ua", "Ukraine", "Ukraine" },
    { "us", "United States", "United States" },
    { "uz", "Uzbekistan", "Uzbekistan" },
    { "za", "South Africa", "South Africa" }
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
    { "am", "amh", NULL, "amh", "Amharic", "አማርኛ" },
    { "ar", "ara", NULL, "ara", "Arabic", "العربية" }, // 30 variations?
    { "az", "aze", NULL, "aze", "Azerbaijani", "Azərbaycanca" },
    { "bg", "bul", NULL, "bul", "Bulgarian", "български език" },
    { "bn", "ben", NULL, "ben", "Bengali", "বাংলা" },
    { "bo", "bod", "tib", "bod", "Tibetan", "བོད་ཡིག" },
    { "ca", "cat", NULL, "cat", "Catalan", "Català" },
    { "cs", "ces", "cze", "ces", "Czech", "česky" },
    { "cy", "wel", "cym", "wel", "Welsh", "Cymraeg" },
    { NULL, "cop", NULL, "cop", "Coptic", "ⲙⲉⲧⲛ̀ⲣⲉⲙⲛ̀ⲭⲏⲙⲓ" },
    { NULL, "csb", NULL, "csb", "Kashubian", "kaszëbsczi jãzëk" },
    { "da", "dan", NULL, "dan", "Danish", "dansk" },
    { "de", "deu", "ger", "deu", "German", "Deutsch" },
    { "el", "gre", "ell", "gre", "Greek", "Ελληνικά" },
    { "en", "eng", NULL, "eng", "English", "English" },
    { "eo", "epo", NULL, "epo", "Esperanto", "Esperanto" },
    { "es", "spa", NULL, "spa", "Spanish", "español" },
    { "et", "est", NULL, "est", "Estonian", "eesti keel" },
    { "eu" "baq", "eus", "baq", "Basque", "euskara" },
    { "fa", "per", "fas", "per", "Persian", "فارسی" },
    { "fi", "fin", NULL, "fin", "Finnish", "suomi" },
    { "fj", "fij", NULL, "fij", "Fijian", "vosa Vakaviti" },
    { "fo", "fao", NULL, "fao", "Faroese", "føroyskt" },
    { "fr", "fra", "fre", "fra", "French", "Français" },
    { "fy", "fry", NULL, "fry", "Frisian", "frysk" },
    { NULL, "fur", NULL, "fur", "Friulian", "furlan" },
    { "ga", "gle", NULL, "gle", "Irish", "Gaeilge" },
    { "gd", "gla", NULL, "gla", "Gaelic", "Gàidhlig" },
    { "gl", "glg", NULL, "glg", "Galician", "Galego" },
    { "he", "heb", NULL, "heb", "Hebrew", "עִבְרִית, עברית" },
    { "hi", "hin", NULL, "hin", "Hindi", "हिन्दी" },
    { "hr", "hrv", NULL, "hrv", "Croatian", "hrvatski jezik" },
    { "hu", "hun", NULL, "hun", "Hungarian", "Magyar" },
    { "hy", "arm", "hye", "arm", "Armenian", "Հայերեն լեզու" },
    { "ia", "ina", NULL, "ina", "Interlingua", "interlingua" },
    { "id", "ind", NULL, "ind", "Indonesian", "Bahasa Indonesia" },
    { "is", "ice", "isl", "ice", "Icelandic", "Icelandic" },
    { "it", "ita", NULL, "ita", "Italian", "Italiano" },
    { "ja", "jpn", NULL, "jpn", "Japanese", "日本語 (にほんご／にっぽんご)" },
    { "km", "khm", NULL, "khm", "Khmer", "ភាសាខ្មែរ" },
    { "ko", "kor", NULL, "kor", "Korean", "한국어 (韓國語), 조선말 (朝鮮語)" },
    { "ku", "kur", NULL, "kur", "Kurdish", "Kurdî" },
    { "la", "lat", NULL, "lat", "Latin", "latine" },
    { "lt", "lit", NULL, "lit", "Lithuanian", "lietuvių kalba" },
    { "lv", "lav", NULL, "lav", "Latvian", "latviešu valoda" },
    { "mg", "mlg", NULL, "mlg", "Malagasy", "Malagasy fiteny" },
    { "mi", "mao", "mri", "mao", "Māori", "te reo Māori" },
    { "mn", "mon", NULL, "mon", "Mongolian", "монгол хэл" },
    { "mr", "mar", NULL, "mar", "Marathi", "मराठी" },
    { "ms", "may", "msa", "may", "Malay", "bahasa Melayu, بهاس ملايو" },
    { "nb", "nob", NULL, "nob", "Norwegian Bokmål", "bokmål" },
    { "ne", "nep", NULL, "nep", "Nepali", "नेपाली" },
    { "nl", "nld", "dut", "nld", "Dutch", "Nederlands" },
    { "nn", "nno", NULL, "nno", "Norwegian Nynorsk", "nynorsk" },
    { "no", "nor", NULL, "nor", "Norwegian", "Norsk" },
    { "nr", "nbl", NULL, "nbl", "South Ndebele", "isiNdebele" },
    { "ny", "nya", NULL, "nya", "Chichewa", "chiCheŵa" },
    { "oc", "oci", NULL, "oci", "Occitan", "Occitan" },
    { "or", "ori", NULL, "ori", "Oriya", "ଓଡ଼ିଆ" },
    { "pa", "pan", NULL, "pan", "Punjabi", "ਪੰਜਾਬੀ" },
    { "pl", "pol", NULL, "pol", "Polish", "polski" },
    { "qu", "que", NULL, "que", "Quechua", "Runa Simi, Kichwa" },
    { "pt", "por", NULL, "por", "Portuguese", "Português" },
    { "ro", "rum", "ron", "rum", "Romanian", "română" },
    { "ru", "rus", NULL, "rus", "Russian", "русский язык" },
    { "rw", "kin", NULL, "kin", "Kinyarwanda", "kinyaRwanda" },
    { "sk", "slk", "slo", "slk", "Slovak", "slovenčina" },
    { "sl", "slv", NULL, "slv", "Slovenian", "slovenščina" },
    { "ss", "ssw", NULL, "ssw", "Swati", "siSwati" },
    { "st", "sot", NULL, "sot", "Sotho", "Sesotho" },
    { "sv", "swe", NULL, "swe", "Swedish", "svenska" },
    { "sw", "swa", NULL, "swa", "Swahili", "kiswahili" },
    { "ta", "tam", NULL, "tam", "Tamil", "தமிழ்" },
    { "th", "tha", NULL, "tha", "Thai", "ภาษาไทย" },
    { "tl", "tgl", NULL, "tgl", "Tagalog", "Tagalog" },
    { "tn", "tsn", NULL, "tsn", "Tswana", "Setswana" },
    //tr tur - tur Turkish Türkçe 
    { "ts", "tso", NULL, "tso", "Tsonga", "Xitsonga" },
    { NULL, "tet", NULL, "tet", "Tetum", "Tetun" },
    { "uk", "ukr", NULL, "ukr", "Ukrainian", "українська мова" },
    { "uz", "uzb", NULL, "uzb", "Uzbek", "O'zbek, Ўзбек, أۇزبېك" },
    { "ve", "ven", NULL, "ven", "Venda", "Tshivenḓa" },
    { "vi", "vie", NULL, "vie", "Vietnamese", "Tiếng Việt" },
    { "xh", "xho", NULL, "xho", "Xhosa", "isiXhosa" },
    //zh zho chi zho + 13 Chinese 中文 (Zhōngwén), 汉语, 漢語 
    { "zu", "zul", NULL, "zul", "Zulu", "isiZulu" }
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
    sLangCode.clear();
    sRegionCode.clear();

    if (!szCode)
        return;

    std::string sCode(szCode);
    
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
    sLanguage.clear();

    if (!szLangCode)
        return;

    std::string sCode(szLangCode);
    const int nCodeSize = sCode.size();
    
    make_lower(sCode);
    
    const char* psz = sCode.c_str();
    
    if (nCodeSize == 2)
    {
        for (size_t i = 0; i < g_languages_size; ++i)
        {
            if (g_languages[i].Level1Code != NULL)
            {
                if (memcmp(psz, g_languages[i].Level1Code, nCodeSize) == 0)
                {
                    sLanguage.assign(g_languages[i].EnglishName);
                    break;
                }
            }
        }
    }
    else if (nCodeSize == 3)
    {
        bool bFound = false;
        
        for (size_t i = 0; i < g_languages_size; ++i)
        {
            if (g_languages[i].Level2CodeT != NULL)
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
    sRegion.clear();

    if (!szRegionCode)
        return;

    std::string sCode(szRegionCode);
    const int nCodeSize = sCode.size();
    
    make_lower(sCode);
    
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

int CIsoLang::Compare(const char* szLhs, const char* szRhs)
{
    if (szLhs == szRhs)
        return 0;

    if (szRhs == 0)
        return 1;

    if (szLhs == 0)
        return -1;

    const size_t szLhsLen = strlen(szLhs);
    const size_t szRhsLen = strlen(szRhs);

    if ((szLhsLen == 0) && (szRhsLen == 0))
        return 0;

    if (szRhsLen == 0)
        return 1;

    if (szLhsLen == 0)
        return -1;

    std::string sLhsCode(szLhs);
    std::string sLhsLangCode;
    std::string sLhsRegionCode;
    std::string sRhsCode(szRhs);
    std::string sRhsLangCode;
    std::string sRhsRegionCode;

    make_lower(sLhsCode);
    make_lower(sRhsCode);

    Parse_SplitCode(sLhsCode.c_str(), sLhsLangCode, sLhsRegionCode);
    Parse_SplitCode(sRhsCode.c_str(), sRhsLangCode, sRhsRegionCode);

    int nCompare = sLhsLangCode.compare(sRhsLangCode);
    if (nCompare == 0)
        nCompare = sLhsRegionCode.compare(sRhsRegionCode);

    return nCompare;
}

int CIsoLang::Compare(const wchar_t* szLhs, const wchar_t* szRhs)
{
    CUTF8Conv u;
    std::string sLhs;
    std::string sRhs;
    u.utf8FromUnicode(szLhs, sLhs);
    u.utf8FromUnicode(szRhs, sRhs);
    return Compare(sLhs.c_str(), sRhs.c_str());
}

void CIsoLang::make_lower(std::string& s)
{
    static std::locale locDefault;
    for ( std::string::iterator it = s.begin();
          it != s.end();
          ++it )
    {
        *it = std::tolower(*it, locDefault);
    }
}

void CIsoLang::GetDisplayName(const char* szCode, std::string& sDisplay)
{
    std::string sLanguage;
    std::string sRegion;

    Parse(szCode, sLanguage, sRegion);

    sDisplay.clear();
    
    if (sLanguage.empty() == false)
    {
        sDisplay = sLanguage; 
        
        if (sRegion.empty() == false)
        {
            sDisplay.append(", ");
            sDisplay.append(sRegion);
        }
    }
}

void CIsoLang::GetDisplayName(const wchar_t* szCode, std::wstring& sDisplay)
{
    CUTF8Conv u;
    std::string s8Code;
    std::string s8Display;

    // Encode parameters
    u.utf8FromUnicode(szCode, s8Code);

    // Make call
    GetDisplayName(s8Code.c_str(), s8Display);

    // Decode results
    u.unicodeFromUtf8(s8Display.c_str(), sDisplay);
}
