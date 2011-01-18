#ifndef _TOLON_SPELLCHECK_INTERNALS_H__
#define _TOLON_SPELLCHECK_INTERNALS_H__

#include "TolonSpellCheck.h"
#include <string>

// Wraps needed for following structures:
// TSC_SESSIONOPTIONS_DATA
// TSC_SHOWOPTIONSWINDOW_DATA
// TSC_CHECKSPELLING_DATA
// TSC_CHECKWORD_DATA
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
// CInitData
///////////////////////////////////////////////////////////////////////////////
    class CInitData
    {
    public:
        CInitData();
        CInitData(const CInitData& src);
        CInitData(const TSC_INIT_DATA& src)
        { FromStruct(src); }
        const CInitData& operator=(const CInitData& rhs);
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
// CVersionData
///////////////////////////////////////////////////////////////////////////////
    class CVersionData
    {
    public:
        CVersionData();
        CVersionData(const CVersionData& src);
        CVersionData(const TSC_VERSION_DATA& src)
        { FromStruct(src); }
        const CVersionData& operator=(const CVersionData& rhs);
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

///////////////////////////////////////////////////////////////////////////////
// CCreateSessionData
///////////////////////////////////////////////////////////////////////////////
    class CCreateSessionData
    {
    public:
        CCreateSessionData();
        CCreateSessionData(const CCreateSessionData& src);
        CCreateSessionData(const TSC_CREATESESSION_DATA& src)
        { FromStruct(src); }
        const CCreateSessionData& operator=(const CCreateSessionData& rhs);
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
        CSessionOptionsData(const TSC_SESSIONOPTIONS_DATA& src)
        { FromStruct(src); }
        const CSessionOptionsData& operator=(const CSessionOptionsData& rhs);
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
        { if (s) m_szDictionaryCulture = s; }

        void PreferredProvider(const wchar_t* s)
        { if (s) m_szPreferredProvider = s; }

        void DefaultLanguage(const wchar_t* s)
        { if (s) m_szDefaultLanguage = s; }

        void CurrentLanguage(const wchar_t* s)
        { if (s) m_szDictionaryCulture = s; }
        
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

};

#endif //_TOLON_SPELLCHECK_INTERNALS_H__
