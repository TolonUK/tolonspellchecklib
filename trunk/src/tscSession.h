#pragma once

#include "TolonSpellCheck.h"
#include "TolonSpellCheckInternals.h"
typedef int ssize_t;
#include "enchant.h"
#include <string>

namespace TolonSpellCheck {

/*class CSessionOptions : public TSC_SESSIONOPTIONS_DATA
{
public:
    CSessionOptions();
    
    // These functions should either stream in and out or they should be able to write to the registry.
    void WriteOut();
    void ReadIn();

    // make this work with abstract data class?
    // m_data.writevalue(x,y), etc.

    void SetLanguage(const char* szCulture);
    
    tsc_result GetCurrentLanguage(wchar_t* ppwszLang) const;
    tsc_result GetCurrentLanguage(char* ppszLang) const;
    const char* GetCurrentLanguage() const
        { return m_sCurrentCulture.c_str(); }

    void SetDefaultLanguage(const char* szLang);
    const char* GetDefaultLanguage() const
        { return &szDictionaryCulture[0]; }

private:
    std::string m_sCurrentCulture;
};*/
    
class CSession
{
public:
    CSession(CCreateSessionData& data);
    ~CSession();

    tsc_result Init();
    tsc_result Uninit();

    // API Support
    tsc_result CheckWord(TSC_CHECKWORD_DATA* pData);
    tsc_result ShowOptionsWindow(TSC_SHOWOPTIONSWINDOW_DATA* pData);
    tsc_result CheckSpelling(TSC_CHECKSPELLING_DATA* pData);
    tsc_result GetSessionOptions(CSessionOptionsData& data);
    tsc_result SetSessionOptions(CSessionOptionsData& data);
	tsc_result CustomDic(TSC_CUSTOMDIC_DATA* pData);

    bool IsInitialised() const
        { return m_bInitialised; }

    void SetInitialised(bool b)
        { m_bInitialised = b; }

    tsc_result SetLanguage(const wchar_t* szCulture);
    
    const wchar_t* GetLanguage() const;
    
    const char* const GetLastError() const
        { return m_szLastError; }
        
private:
	// Implementation
	void FreeCurrentDictionary();

	// Response methods
    tsc_result Error_NotImplemented();
    tsc_result Error_Internal_NullModulePtr();
	tsc_result Error_InvalidArg();
    tsc_result Error_ParamWasNull();
    tsc_result Error_SessionAlreadyInitialised();
    tsc_result Error_SessionNotInitialised();
    tsc_result Error_StructSizeInvalid();
    tsc_result Success();

private:
    bool m_bInitialised;
    EnchantDict* m_pEnchantDict;
    CSessionOptionsData m_options;
    const char* m_szLastError;
    std::wstring m_szCurrentCulture;
};

} //namespace