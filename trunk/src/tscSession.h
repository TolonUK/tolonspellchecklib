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
	tsc_result CheckWord(const char* szWord);

    tsc_result ShowOptionsWindow();

	bool IsInitialised() const
		{ return m_bInitialised; }

	void SetInitialised(bool b)
		{ m_bInitialised = b; }
	
	tsc_result GetCurrentLanguage(const wchar_t* ppwszLang) const;
	tsc_result GetCurrentLanguage(const char* ppszLang) const;
		
	tsc_result DescribeLanguage(const wchar_t* wszLang, LANGUAGE_DESC_WIDEDATA* pData) const;
	tsc_result DescribeLanguage(const char* szLang, LANGUAGE_DESC_DATA* pData) const;

protected:
	bool m_bInitialised;
	EnchantBroker* m_pEnchantBroker;
	EnchantDict* m_pEnchantDict;
    TSC_SESSIONOPTIONS_DATA m_options;
};

} //namespace