#pragma once

#include "TolonSpellCheck.h"
#include "TolonSpellCheckInternals.h"
typedef int ssize_t;
#include "enchant.h"
#include <map>
#include <string>

#pragma comment(linker, \
    "\"/manifestdependency:type='Win32' "\
    "name='Microsoft.Windows.Common-Controls' "\
    "version='6.0.0.0' "\
    "processorArchitecture='*' "\
    "publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

namespace TolonSpellCheck {

class CSession;

class CModule
{
public:
    static CModule* GetInstance();
 
    static void cbEnchantDictDescribe( const char * const lang_tag,
                                         const char * const provider_name,
                                         const char * const provider_desc,
                                         const char * const provider_file,
                                         void * user_data );

protected:
    CModule();
    ~CModule();

public:
	tsc_result Init(TSC_INIT_DATA* pData);
	tsc_result Uninit();

	bool IsInitialised() const
		{ return m_bInitialised; }
		
	const char* const GetLastError() const
		{ return m_szLastError; }
		
	tsc_result GetVersion( TSC_VERSION_DATA* pData );
	tsc_result CreateSession( tsc_cookie* pSessionID,
							  TSC_CREATESESSION_DATA* pData );
	tsc_result DestroySession( tsc_cookie SessionID );
	tsc_result GetSessionOptions( tsc_cookie SessionID, 
								  TSC_SESSIONOPTIONS_DATA* pData );
	tsc_result SetSessionOptions( tsc_cookie SessionID,
								  TSC_SESSIONOPTIONS_DATA* pData );
	tsc_result ShowOptionsWindow( tsc_cookie SessionID,
								  TSC_SHOWOPTIONSWINDOW_DATA* pData );
	tsc_result CheckSpelling( tsc_cookie SessionID,
							  TSC_CHECKSPELLING_DATA* pData );
	tsc_result CheckWord( tsc_cookie SessionID,
						  TSC_CHECKWORD_DATA* pData );

    tsc_result EnumLanguages(LanguageEnumFn pfn, void* pUserData);

    // Enchant Functions
    EnchantDict* GetDictionary(const char* szCulture);
    void FreeDictionary(EnchantDict* const pDict);

protected:
	void SetInitialised(bool b)
		{ m_bInitialised = b; }
		
	CSession* GetSession(tsc_cookie SessionID);
		
	tsc_cookie GetNextSessionCookie();

	tsc_result DescribeLanguage(const wchar_t* wszLang, LANGUAGE_DESC_WIDEDATA* pData);
	tsc_result DescribeLanguage(const char* szLang, LANGUAGE_DESC_DATA* pData);

//Error methods
protected:
    tsc_result Error_FailedToInitEnchant();
	tsc_result Error_InvalidSessionCookie();
	tsc_result Error_ModuleAlreadyInitialised();
	tsc_result Error_ModuleNotInitialised();
	tsc_result Error_NotImplemented();
	tsc_result Error_ParamWasNull();
    tsc_result Error_SessionCreationFailed();
	tsc_result Error_StructSizeInvalid();
	tsc_result Success();

// Member variables
private:
    struct EnumLanguagesPayload {
        LanguageEnumFn pfn;
        void* pUserData;
        CModule* pThis; };

private:
	static CModule* sm_pThis;

	std::string m_sHostName;
	bool m_bInitialised;
	const char* m_szLastError;
	std::map<tsc_cookie, CSession*> m_xSessions;
	tsc_cookie m_nLastSessionCookie;
    EnchantBroker* m_pEnchantBroker;
};

}; //namespace