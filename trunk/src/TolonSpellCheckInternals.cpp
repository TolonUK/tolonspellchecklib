#include "TolonSpellCheckInternals.h"
#include "utf8conv.h"
#include <windows.h>
#include <cassert>

using namespace TolonSpellCheck;

///////////////////////////////////////////////////////////////////////////////
// CCheckWordData
///////////////////////////////////////////////////////////////////////////////
CCheckWordData::CCheckWordData()
{
}

CCheckWordData::CCheckWordData(const CCheckWordData& src) :
    m_sTestWord(src.m_sTestWord),
    m_vResults(src.m_vResults)
{
}

const CCheckWordData& CCheckWordData::operator=(const CCheckWordData& rhs)
{
    m_sTestWord = rhs.m_sTestWord;
    m_vResults = rhs.m_vResults;

    return *this;
}

bool CCheckWordData::operator==(const CCheckWordData& rhs) const
{
    return (m_sTestWord == rhs.m_sTestWord) &&
           (m_vResults == rhs.m_vResults);
}

void CCheckWordData::ToStruct(TSC_CHECKWORD_DATA& dest) const
{
    dest.cbSize = sizeof(TSC_CHECKWORD_DATA);
    //utf8_from_string(dest.szAppName, sizeof(dest.szAppName), m_szAppName);
    //TODO: Coding to struct
}

void CCheckWordData::FromStruct(const TSC_CHECKWORD_DATA& src)
{
    assert(src.cbSize == sizeof(TSC_CHECKWORD_DATA));
    //string_from_utf8(m_szAppName, src.szAppName, sizeof(src.szAppName));
    //TODO: Coding from struct
}

CCheckWordData& TolonSpellCheck::operator<<(CCheckWordData& dest, const TSC_CHECKWORD_DATA& src)
{
    dest.FromStruct(src);
    return dest;
}

TSC_CHECKWORD_DATA& TolonSpellCheck::operator<<(TSC_CHECKWORD_DATA& dest, const CCheckWordData& src)
{
    src.ToStruct(dest);
    return dest;
}

///////////////////////////////////////////////////////////////////////////////
// CInitData
///////////////////////////////////////////////////////////////////////////////
CInitData::CInitData()
{
}

CInitData::CInitData(const CInitData& src) :
    m_szAppName(src.m_szAppName)
{
}

const CInitData& CInitData::operator=(const CInitData& rhs)
{
    m_szAppName = rhs.m_szAppName;

    return *this;
}

bool CInitData::operator==(const CInitData& rhs) const
{
    if (this == &rhs)
        return true;

    return m_szAppName == rhs.m_szAppName;
}

void CInitData::ToStruct(TSC_INIT_DATA& dest) const
{
    dest.cbSize = sizeof(TSC_INIT_DATA);
    utf8_from_string(dest.szAppName, sizeof(dest.szAppName), m_szAppName);
}

void CInitData::FromStruct(const TSC_INIT_DATA& src)
{
    assert(src.cbSize == sizeof(TSC_INIT_DATA));
    string_from_utf8(m_szAppName, src.szAppName, sizeof(src.szAppName));
}

CInitData& TolonSpellCheck::operator<<(CInitData& dest, const TSC_INIT_DATA& src)
{
    dest.FromStruct(src);
    return dest;
}

TSC_INIT_DATA& TolonSpellCheck::operator<<(TSC_INIT_DATA& dest, const CInitData& src)
{
    src.ToStruct(dest);
    return dest;
}

///////////////////////////////////////////////////////////////////////////////
// CCreateSessionData
///////////////////////////////////////////////////////////////////////////////
CCreateSessionData::CCreateSessionData()
{
}

CCreateSessionData::CCreateSessionData(const CCreateSessionData& src)
{
}

const CCreateSessionData& CCreateSessionData::operator=(const CCreateSessionData& rhs)
{
    return *this;
}

bool CCreateSessionData::operator==(const CCreateSessionData& rhs) const
{
    return true;
}

void CCreateSessionData::ToStruct(TSC_CREATESESSION_DATA& dest) const
{
    dest.cbSize = sizeof(TSC_CREATESESSION_DATA);
}

void CCreateSessionData::FromStruct(const TSC_CREATESESSION_DATA& src)
{
    assert(src.cbSize == sizeof(TSC_CREATESESSION_DATA));
}

CCreateSessionData& TolonSpellCheck::operator<<(CCreateSessionData& dest, const TSC_CREATESESSION_DATA& src)
{
    dest.FromStruct(src);
    return dest;
}

TSC_CREATESESSION_DATA& TolonSpellCheck::operator<<(TSC_CREATESESSION_DATA& dest, const CCreateSessionData& src)
{
    src.ToStruct(dest);
    return dest;
}

///////////////////////////////////////////////////////////////////////////////
// CSessionOptionsData
///////////////////////////////////////////////////////////////////////////////
CSessionOptionsData::CSessionOptionsData() :
    m_bIgnoreUserDictionaries(false),
    m_bIgnoreUppercaseWords(false),
    m_bIgnoreWordsWithNumbers(true),
    m_bIgnoreUris(true)
{
}

CSessionOptionsData::CSessionOptionsData(const CSessionOptionsData& src) :
    m_bIgnoreUserDictionaries(src.m_bIgnoreUserDictionaries),
    m_bIgnoreUppercaseWords(src.m_bIgnoreUppercaseWords),
    m_bIgnoreWordsWithNumbers(src.m_bIgnoreWordsWithNumbers),
    m_bIgnoreUris(src.m_bIgnoreUris),
    m_szDictionaryCulture(src.m_szDictionaryCulture),
    m_szPreferredProvider(src.m_szPreferredProvider),
    m_szDefaultLanguage(src.m_szDefaultLanguage)
{
}

const CSessionOptionsData& CSessionOptionsData::operator=(const CSessionOptionsData& rhs)
{
    m_bIgnoreUserDictionaries = rhs.m_bIgnoreUserDictionaries;
    m_bIgnoreUppercaseWords = rhs.m_bIgnoreUppercaseWords;
    m_bIgnoreWordsWithNumbers = rhs.m_bIgnoreWordsWithNumbers;
    m_bIgnoreUris = rhs.m_bIgnoreUris;
    m_szDictionaryCulture = rhs.m_szDictionaryCulture;
    m_szPreferredProvider = rhs.m_szPreferredProvider;
    m_szDefaultLanguage = rhs.m_szDefaultLanguage;

    return *this;
}

bool CSessionOptionsData::operator==(const CSessionOptionsData& rhs) const
{
    if (this == &rhs)
        return true;

    return 
        (m_bIgnoreUserDictionaries == rhs.m_bIgnoreUserDictionaries) &&
        (m_bIgnoreUppercaseWords == rhs.m_bIgnoreUppercaseWords) &&
        (m_bIgnoreWordsWithNumbers == rhs.m_bIgnoreWordsWithNumbers) &&
        (m_bIgnoreUris == rhs.m_bIgnoreUris) &&
        (m_szDictionaryCulture == rhs.m_szDictionaryCulture) &&
        (m_szPreferredProvider == rhs.m_szPreferredProvider) &&
        (m_szDefaultLanguage == rhs.m_szDefaultLanguage);
}

void CSessionOptionsData::ToStruct(TSC_SESSIONOPTIONS_DATA& dest) const
{
    dest.cbSize = sizeof(TSC_SESSIONOPTIONS_DATA);
    dest.bIgnoreUserDictionaries = m_bIgnoreUserDictionaries;
    dest.bIgnoreUppercaseWords = m_bIgnoreUppercaseWords;
    dest.bIgnoreWordsWithNumbers = m_bIgnoreWordsWithNumbers;
    dest.bIgnoreUris = m_bIgnoreUris;
    utf8_from_string(dest.szDictionaryCulture, sizeof(dest.szDictionaryCulture), m_szDictionaryCulture);
    utf8_from_string(dest.szPreferredProvider, sizeof(dest.szPreferredProvider), m_szPreferredProvider);
    // Default language not stored here?
}

void CSessionOptionsData::FromStruct(const TSC_SESSIONOPTIONS_DATA& src)
{
    assert(src.cbSize == sizeof(TSC_SESSIONOPTIONS_DATA));
    m_bIgnoreUserDictionaries = (src.bIgnoreUserDictionaries != 0);
    m_bIgnoreUppercaseWords = (src.bIgnoreUppercaseWords != 0);
    m_bIgnoreWordsWithNumbers = (src.bIgnoreWordsWithNumbers != 0);
    m_bIgnoreUris = (src.bIgnoreUris != 0);
    string_from_utf8(m_szDictionaryCulture, src.szDictionaryCulture, sizeof(src.szDictionaryCulture));
    string_from_utf8(m_szPreferredProvider, src.szPreferredProvider, sizeof(src.szPreferredProvider));
    m_szDefaultLanguage = m_szDictionaryCulture;
}

void CSessionOptionsData::SetDefaultLanguageFromOS()
{
    // win98 and later
    int n = 0;
    const int LOCALE_BUFLEN = 20;
    const int LOCALE_FIRSTPART = 10;
    const int LOCALE_SECONDPART = 10;
    wchar_t wszBuf[LOCALE_BUFLEN] = {0};
    char szBuf[LOCALE_BUFLEN] = {0};

    n = ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, wszBuf, LOCALE_FIRSTPART);
    if (n != 0)
    {
        wcscat_s(wszBuf, LOCALE_BUFLEN, L"-");
        n = ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, wszBuf + wcslen(wszBuf), LOCALE_SECONDPART);
    }

    m_szDefaultLanguage = wszBuf;

    if (m_szDictionaryCulture.empty())
        m_szDictionaryCulture = m_szDefaultLanguage;
}

CSessionOptionsData& TolonSpellCheck::operator<<(CSessionOptionsData& dest, const TSC_SESSIONOPTIONS_DATA& src)
{
    dest.FromStruct(src);
    return dest;
}

TSC_SESSIONOPTIONS_DATA& TolonSpellCheck::operator<<(TSC_SESSIONOPTIONS_DATA& dest, const CSessionOptionsData& src)
{
    src.ToStruct(dest);
    return dest;
}

std::wostream& TolonSpellCheck::operator<<(std::wostream& os, const CSessionOptionsData& src)
{
    os << L"CSessionOptionsData("
       << L" m_bIgnoreUserDictionaries := " << src.IgnoreUserDictionaries()
       << L", m_bIgnoreUppercaseWords := " << src.IgnoreUppercaseWords()
       << L", m_bIgnoreWordsWithNumbers := " << src.IgnoreWordsWithNumbers()
       << L", m_bIgnoreUris := " << src.IgnoreUris()
       << L", m_szDictionaryCulture := " << src.DictionaryCulture()
       << L", m_szPreferredProvider := " << src.PreferredProvider()
       << L", m_szDefaultLanguage := " << src.DefaultLanguage() << L")";

    return os;
}

///////////////////////////////////////////////////////////////////////////////
// CShowOptionsWindowData
///////////////////////////////////////////////////////////////////////////////
CShowOptionsWindowData::CShowOptionsWindowData() :
    m_hParent(NULL)
{
}

CShowOptionsWindowData::CShowOptionsWindowData(const CShowOptionsWindowData& src) :
    m_hParent(src.m_hParent)
{
}

const CShowOptionsWindowData& CShowOptionsWindowData::operator=(const CShowOptionsWindowData& rhs)
{
    m_hParent = rhs.m_hParent;

    return *this;
}

bool CShowOptionsWindowData::operator==(const CShowOptionsWindowData& rhs) const
{
    if (this == &rhs)
        return true;

    return 
        (m_hParent == rhs.m_hParent);
}

void CShowOptionsWindowData::ToStruct(TSC_SHOWOPTIONSWINDOW_DATA& dest) const
{
    dest.cbSize = sizeof(TSC_SESSIONOPTIONS_DATA);
    dest.hwndParent = m_hParent;
}

void CShowOptionsWindowData::FromStruct(const TSC_SHOWOPTIONSWINDOW_DATA& src)
{
    assert(src.cbSize == sizeof(TSC_SHOWOPTIONSWINDOW_DATA));
    m_hParent = src.hwndParent;
}

CShowOptionsWindowData::operator TSC_SHOWOPTIONSWINDOW_DATA* ()
{
    ToStruct(m_xHelper);
    return &m_xHelper;
}

CShowOptionsWindowData& TolonSpellCheck::operator<<(CShowOptionsWindowData& dest, const TSC_SHOWOPTIONSWINDOW_DATA& src)
{
    dest.FromStruct(src);
    return dest;
}

TSC_SHOWOPTIONSWINDOW_DATA& TolonSpellCheck::operator<<(TSC_SHOWOPTIONSWINDOW_DATA& dest, const CShowOptionsWindowData& src)
{
    src.ToStruct(dest);
    return dest;
}

std::wostream& TolonSpellCheck::operator<<(std::wostream& os, const CShowOptionsWindowData& src)
{
    os << L"CShowOptionsWindowData("
       << L" m_hParent := 0x" << std::hex << src.ParentWindow() << L")";

    return os;
}

///////////////////////////////////////////////////////////////////////////////
// CVersionData
///////////////////////////////////////////////////////////////////////////////
CVersionData::CVersionData() :
    m_nMajor(TSC_VERSION_MAJOR),
    m_nMinor(TSC_VERSION_MINOR)
{
}

CVersionData::CVersionData(const CVersionData& src) :
    m_nMajor(src.m_nMajor),
    m_nMinor(src.m_nMinor)
{
}

const CVersionData& CVersionData::operator=(const CVersionData& rhs)
{
    m_nMajor = rhs.m_nMajor;
    m_nMinor = rhs.m_nMinor;

    return *this;
}

bool CVersionData::operator==(const CVersionData& rhs) const
{
    if (this == &rhs)
        return true;

    return (m_nMajor == rhs.m_nMajor) && (m_nMinor == rhs.m_nMinor);
}

void CVersionData::ToStruct(TSC_VERSION_DATA& dest) const
{
    dest.cbSize = sizeof(TSC_VERSION_DATA);
    dest.nMajor = m_nMajor;
    dest.nMinor = m_nMinor;
    dest.nReserved1 = 0;
    dest.nReserved2 = 0;
}

void CVersionData::FromStruct(const TSC_VERSION_DATA& src)
{
    assert(src.cbSize == sizeof(TSC_VERSION_DATA));
    m_nMajor = src.nMajor;
    m_nMinor = src.nMinor;
}

CVersionData& TolonSpellCheck::operator<<(CVersionData& dest, const TSC_VERSION_DATA& src)
{
    dest.FromStruct(src);
    return dest;
}

TSC_VERSION_DATA& TolonSpellCheck::operator<<(TSC_VERSION_DATA& dest, const CVersionData& src)
{
    src.ToStruct(dest);
    return dest;
}
