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
    { "am", "Armenia", "Armenia" },
    { "at", "Austria", "Austria" },
    { "au", "Australia", "Australia" },
    { "bg", "Bulgaria", "Bulgaria" },
    { "bo", "Bolivia", "Bolivia" },
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
    { "ro", "Romania", "Romania" },
    { "ru", "Russia", "Russia" },
    { "rw", "Rwanda", "Rwanda" },
    { "tr", "Turkey", "Turkey" },
    { "us", "United States", "United States" },
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
    { "ne" "nep", NULL, "nep", "Nepali", "नेपाली" },
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
   // pt por - por Portuguese Português 
    { "ro", "rum", "ron", "rum", "Romanian", "română" },
    { "ru", "rus", NULL, "rus", "Russian", "русский язык" },
    { "rw", "kin", NULL, "kin", "Kinyarwanda", "kinyaRwanda" }
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
