#pragma once

#include "TolonSpellCheck.h"
#include "TolonSpellCheckInternals.h"
typedef int ssize_t;
#include "enchant.h"

namespace TolonSpellCheck {
    
class CSession
{
public:
    static void cbEnchantDictDescribe( const char * const lang_tag,
                                         const char * const provider_name,
                                         const char * const provider_desc,
                                         const char * const provider_file,
                                         void * user_data );
public:
    CSession(TSC_CREATESESSION_DATA* pData);
    ~CSession();

	tsc_result Init();
	tsc_result Uninit();

	// API Support
	tsc_result CheckWord(TSC_CHECKWORD_DATA* pData);
	tsc_result ShowOptionsWindow(TSC_SHOWOPTIONSWINDOW_DATA* pData);
	tsc_result CheckSpelling(TSC_CHECKSPELLING_DATA* pData);
	tsc_result GetSessionOptions(TSC_SESSIONOPTIONS_DATA* pData);
	tsc_result SetSessionOptions(TSC_SESSIONOPTIONS_DATA* pData);

	bool IsInitialised() const
		{ return m_bInitialised; }

	void SetInitialised(bool b)
		{ m_bInitialised = b; }
	
	tsc_result GetCurrentLanguage(wchar_t* ppwszLang);
	tsc_result GetCurrentLanguage(char* ppszLang);
		
	tsc_result DescribeLanguage(const wchar_t* wszLang, LANGUAGE_DESC_WIDEDATA* pData);
	tsc_result DescribeLanguage(const char* szLang, LANGUAGE_DESC_DATA* pData);
		
	const char* const GetLastError() const
		{ return m_szLastError; }
		
	tsc_result EnumLanguages(LanguageEnumFn pfn, void* pUserData);

private:
    struct EnumLanguagesPayload {
        LanguageEnumFn pfn;
        void* pUserData;
        CSession* pThis; };
        
private:
	tsc_result Error_NotImplemented();
	tsc_result Error_ParamWasNull();
	tsc_result Error_SessionAlreadyInitialised();
	tsc_result Error_SessionNotInitialised();
	tsc_result Error_StructSizeInvalid();
	tsc_result Success();

private:
	bool m_bInitialised;
	EnchantBroker* m_pEnchantBroker;
	EnchantDict* m_pEnchantDict;
	TSC_SESSIONOPTIONS_DATA m_options;
	const char* m_szLastError;
};

} //namespace