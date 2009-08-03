#include "isoLang.h"

// http://www.loc.gov/standards/iso639-2/php/English_list.php
// http://en.wikipedia.org/wiki/List_of_ISO_639-2_codes

struct ISO3166Region {
    wchar_t* Alpha2Code;
    wchar_t* EnglishName;
    char* NativeName;
};

ISO3166Region {
    { L"fr", L"France", "France" },
    { L"gb", L"United Kingdom", "United Kingdom" },
    { L"us", L"United States", "United States" }
};

struct ISO639Language {
    wchar_t* Level1Code;
    wchar_t* Level2CodeT;
    wchar_t* Level2CodeB;
    wchar_t* Level3Code;
    wchar_t* EnglishName;
    char* NativeName; //UTF8
};

ISO639Language g_languages[] = {
    { L"bo",	L"bod",	L"tib", L"bod", L"Tibetan", "བོད་ཡིག" },
    { L"en", L"eng", NULL, L"eng", L"English", "English" },
    { L"fr", L"fra", L"fre", L"fra", L"French", "Français" },
    { L"ja", L"jpn", NULL, L"jpn", L"Japanese", "日本語 (にほんご／にっぽんご)" }
};	

