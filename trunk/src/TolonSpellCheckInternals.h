#ifndef _TOLON_SPELLCHECK_INTERNALS_H__
#define _TOLON_SPELLCHECK_INTERNALS_H__

#include "TolonSpellCheck.h"
#include <algorithm>
#include <string>
#include <vector>

// Wraps needed for following structures:
// TSC_CHECKSPELLING_DATA
// LANGUAGE_DESC_DATA
// LANGUAGE_DESC_WIDEDATA

namespace TolonSpellCheck {
    typedef struct tagLANGUAGE_DESC {
        tsc_size_t cbSize;
        char szCodeName[13];
        char szDisplayName[128];
    } LANGUAGE_DESC_DATA;
    
    typedef struct tagLANGUAGE_DESC_WIDE {
        tsc_size_t cbSize;
        wchar_t wszCodeName[13];
        wchar_t wszDisplayName[128];
    } LANGUAGE_DESC_WIDEDATA;
    
    class CModule;
    class CSession;
    
    typedef bool (*LanguageEnumFn) (LANGUAGE_DESC_WIDEDATA* pData,
                                      void * user_data);

///////////////////////////////////////////////////////////////////////////////
// CCheckWordData
///////////////////////////////////////////////////////////////////////////////
    class CCheckWordData
    {
    public:
        CCheckWordData();
        CCheckWordData(const CCheckWordData& src);
        CCheckWordData(const TSC_CHECKWORD_DATA& src) { FromStruct(src); }
        CCheckWordData& operator=(const CCheckWordData& rhs);
        bool operator==(const CCheckWordData& rhs) const;

        void ToStruct(TSC_CHECKWORD_DATA& dest);
        void FromStruct(const TSC_CHECKWORD_DATA& src);

        void TestWord(const wchar_t* s)
        { if (s) m_sTestWord = s; 
          else m_sTestWord.clear(); }
        const wchar_t* TestWord() const
        { return m_sTestWord.c_str(); }

        template <typename Fn>
        Fn Call(Fn fn)
        {
            ToStruct(m_xHelper);
            fn(&m_xHelper);
            FromStruct(m_xHelper);
            return fn;
        }

        template <typename Fn>
        Fn ApplyResults(Fn fn)
        {
            return std::for_each(m_vResults.begin(), m_vResults.end(), fn);
        }

        /*template <typename It>
        It IterateResults(It i)
        {
            return std::copy(m_vResults.begin(), m_vResults.end(), i);
        }*/

        bool WordWasOk() const
        { return m_bWordOk; }

    private:
        void ResultsToStruct(TSC_CHECKWORD_DATA& dest);
        void ResultsFromStruct(const TSC_CHECKWORD_DATA& src);

    private:
        // Struct representation in unicode
        std::wstring m_sTestWord;
        std::vector<std::wstring> m_vResults;
        bool m_bWordOk;

        // Struct representation in utf-8
        std::string m_sTestWordUtf8;
        std::vector<char> m_vResultsUtf8;

        bool m_bResultsUtf8Locked;

        TSC_CHECKWORD_DATA m_xHelper;
    };

    CCheckWordData& operator<<(CCheckWordData& dest, const TSC_CHECKWORD_DATA& src);
    TSC_CHECKWORD_DATA& operator<<(TSC_CHECKWORD_DATA& dest, const CCheckWordData& src);

///////////////////////////////////////////////////////////////////////////////
// CCustomDicData
///////////////////////////////////////////////////////////////////////////////
    class CCustomDicData
    {
    public:
        CCustomDicData();
        CCustomDicData(const CCustomDicData& src);
        CCustomDicData(const TSC_CUSTOMDIC_DATA& src) { FromStruct(src); }
        CCustomDicData& operator=(const CCustomDicData& rhs);
        bool operator==(const CCustomDicData& rhs) const;

        void ToStruct(TSC_CUSTOMDIC_DATA& dest);
        void FromStruct(const TSC_CUSTOMDIC_DATA& src);

        template <typename Fn>
        Fn Call(Fn fn)
        {
            ToStruct(m_xHelper);
            fn(&m_xHelper);
            FromStruct(m_xHelper);
            return fn;
        }

    private:
        void ResultsToStruct(TSC_CUSTOMDIC_DATA& dest);
        void ResultsFromStruct(const TSC_CUSTOMDIC_DATA& src);

    private:
        // Struct representation in unicode
        std::wstring m_sWord;

        // Struct representation in utf-8
        std::string m_sWordUtf8;

        TSC_CUSTOMDIC_DATA m_xHelper;
    };

    CCustomDicData& operator<<(CCustomDicData& dest, const TSC_CUSTOMDIC_DATA& src);
    TSC_CUSTOMDIC_DATA& operator<<(TSC_CUSTOMDIC_DATA& dest, const CCustomDicData& src);

///////////////////////////////////////////////////////////////////////////////
// CInitData
///////////////////////////////////////////////////////////////////////////////
    class CInitData
    {
    public:
        CInitData();
        CInitData(const CInitData& src);
        CInitData(const TSC_INIT_DATA& src) { FromStruct(src); }
        CInitData& operator=(const CInitData& rhs);
        bool operator==(const CInitData& rhs) const;

        void ToStruct(TSC_INIT_DATA& dest) const;
        void FromStruct(const TSC_INIT_DATA& src);

        const wchar_t* AppName() const
        { return m_szAppName.c_str(); }
    
    private:
        std::wstring m_szAppName;
    };

    // << operator definitions
    CInitData& operator<<(CInitData& dest, const TSC_INIT_DATA& src);
    TSC_INIT_DATA& operator<<(TSC_INIT_DATA& dest, const CInitData& src);

///////////////////////////////////////////////////////////////////////////////
// CCreateSessionData
///////////////////////////////////////////////////////////////////////////////
    class CCreateSessionData
    {
    public:
        CCreateSessionData();
        CCreateSessionData(const CCreateSessionData& src);
        CCreateSessionData(const TSC_CREATESESSION_DATA& src) { FromStruct(src); }
        CCreateSessionData& operator=(const CCreateSessionData& rhs);
        bool operator==(const CCreateSessionData& rhs) const;

        void ToStruct(TSC_CREATESESSION_DATA& dest) const;
        void FromStruct(const TSC_CREATESESSION_DATA& src);

    private:
    };

    // << operator definitions
    CCreateSessionData& operator<<(CCreateSessionData& dest, const TSC_CREATESESSION_DATA& src);
    TSC_CREATESESSION_DATA& operator<<(TSC_CREATESESSION_DATA& dest, const CCreateSessionData& src);

///////////////////////////////////////////////////////////////////////////////
// CSessionOptionsData
///////////////////////////////////////////////////////////////////////////////
    class CSessionOptionsData
    {
    public:
        CSessionOptionsData();
        CSessionOptionsData(const CSessionOptionsData& src);
        CSessionOptionsData(const TSC_SESSIONOPTIONS_DATA& src) { FromStruct(src); }
        CSessionOptionsData& operator=(const CSessionOptionsData& rhs);
        bool operator==(const CSessionOptionsData& rhs) const;

        void ToStruct(TSC_SESSIONOPTIONS_DATA& dest) const;
        void FromStruct(const TSC_SESSIONOPTIONS_DATA& src);

        // Getter methods
        const wchar_t* DictionaryCulture() const
        { return m_szDictionaryCulture.c_str(); }

        const wchar_t* PreferredProvider() const
        { return m_szPreferredProvider.c_str(); }

        const wchar_t* DefaultLanguage() const
        { return m_szDefaultLanguage.c_str(); }

        const wchar_t* CurrentLanguage() const
        { return m_szDictionaryCulture.c_str(); }
        
        bool IgnoreUserDictionaries() const
        { return m_bIgnoreUserDictionaries; }
        
        bool IgnoreUppercaseWords() const
        { return m_bIgnoreUppercaseWords; }
        
        bool IgnoreWordsWithNumbers() const
        { return m_bIgnoreWordsWithNumbers; }
        
        bool IgnoreUris() const
        { return m_bIgnoreUris; }

        // Setter methods
        void DictionaryCulture(const wchar_t* s)
        { if (s) m_szDictionaryCulture = s;
          else m_szDictionaryCulture.clear(); }

        void PreferredProvider(const wchar_t* s)
        { if (s) m_szPreferredProvider = s;
          else m_szPreferredProvider.clear(); }

        void DefaultLanguage(const wchar_t* s)
        { if (s) m_szDefaultLanguage = s;
          else m_szDefaultLanguage.clear(); }

        void CurrentLanguage(const wchar_t* s)
        { if (s) m_szDictionaryCulture = s;
          else m_szDictionaryCulture.clear(); }
        
        void IgnoreUserDictionaries(bool b)
        { m_bIgnoreUserDictionaries = b; }
        
        void IgnoreUppercaseWords(bool b)
        { m_bIgnoreUppercaseWords = b; }
        
        void IgnoreWordsWithNumbers(bool b)
        { m_bIgnoreWordsWithNumbers = b; }
        
        void IgnoreUris(bool b)
        { m_bIgnoreUris = b; }

        // Other methods

        bool HasDefaultLanguage() const
        { return !m_szDefaultLanguage.empty(); }

        void SetDefaultLanguageFromOS();
    
    private:
        // Struct members
        bool m_bIgnoreUserDictionaries;
        bool m_bIgnoreUppercaseWords;
        bool m_bIgnoreWordsWithNumbers;
        bool m_bIgnoreUris;
        std::wstring m_szDictionaryCulture;
        std::wstring m_szPreferredProvider;
        std::wstring m_szDefaultLanguage;
    };

    // << operator definitions
    CSessionOptionsData& operator<<(CSessionOptionsData& dest, const TSC_SESSIONOPTIONS_DATA& src);
    TSC_SESSIONOPTIONS_DATA& operator<<(TSC_SESSIONOPTIONS_DATA& dest, const CSessionOptionsData& src);
    std::wostream& operator<<(std::wostream& os, const CSessionOptionsData& src);

///////////////////////////////////////////////////////////////////////////////
// CShowOptionsWindowData
///////////////////////////////////////////////////////////////////////////////
    class CShowOptionsWindowData
    {
    public:
        CShowOptionsWindowData();
        CShowOptionsWindowData(const CShowOptionsWindowData& src);
        CShowOptionsWindowData(const TSC_SHOWOPTIONSWINDOW_DATA& src) { FromStruct(src); }
        CShowOptionsWindowData& operator=(const CShowOptionsWindowData& rhs);
        bool operator==(const CShowOptionsWindowData& rhs) const;

        void ToStruct(TSC_SHOWOPTIONSWINDOW_DATA& dest) const;
        void FromStruct(const TSC_SHOWOPTIONSWINDOW_DATA& src);

        // Implicit conversion
        operator TSC_SHOWOPTIONSWINDOW_DATA* ();

        // Getter methods
        HWND ParentWindow() const
        { return m_hParent; }

        // Setter methods
        void ParentWindow(HWND h)
        { m_hParent = h; }
    
    private:
        // Struct members
        HWND m_hParent;

        // Helper struct for implicit conversion
        TSC_SHOWOPTIONSWINDOW_DATA m_xHelper;
    };

    // << operator definitions
    CShowOptionsWindowData& operator<<(CShowOptionsWindowData& dest, const TSC_SHOWOPTIONSWINDOW_DATA& src);
    TSC_SHOWOPTIONSWINDOW_DATA& operator<<(TSC_SHOWOPTIONSWINDOW_DATA& dest, const CShowOptionsWindowData& src);
    std::wostream& operator<<(std::wostream& os, const CShowOptionsWindowData& src);

///////////////////////////////////////////////////////////////////////////////
// CVersionData
///////////////////////////////////////////////////////////////////////////////
    class CVersionData
    {
    public:
        CVersionData();
        CVersionData(const CVersionData& src);
        CVersionData(const TSC_VERSION_DATA& src) { FromStruct(src); }
        CVersionData& operator=(const CVersionData& rhs);
        bool operator==(const CVersionData& rhs) const;

        void ToStruct(TSC_VERSION_DATA& dest) const;
        void FromStruct(const TSC_VERSION_DATA& src);

    private:
        unsigned char m_nMajor;
        unsigned char m_nMinor;
    };

    // << operator definitions
    CVersionData& operator<<(CVersionData& dest, const TSC_VERSION_DATA& src);
    TSC_VERSION_DATA& operator<<(TSC_VERSION_DATA& dest, const CVersionData& src);


};


#endif //_TOLON_SPELLCHECK_INTERNALS_H__
