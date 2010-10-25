#pragma once

#include "TolonSpellCheck.h"
#include <map>
#include <string>

namespace TolonSpellCheck {

class CSession;

class CModule
{
public:
    static CModule* GetInstance();
    
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

protected:
	void SetInitialised(bool b)
		{ m_bInitialised = b; }
		
	CSession* GetSession(tsc_cookie SessionID);
		
	tsc_cookie GetNextSessionCookie();
		
//Error methods
protected:
	tsc_result Error_InvalidSessionCookie();
	tsc_result Error_ModuleAlreadyInitialised();
	tsc_result Error_ModuleNotInitialised();
	tsc_result Error_NotImplemented();
	tsc_result Error_ParamWasNull();
    tsc_result Error_SessionCreationFailed();
	tsc_result Error_StructSizeInvalid();
	tsc_result Success();

// Member variables
protected:
	static CModule* sm_pThis;

	std::string m_sHostName;
	bool m_bInitialised;
	const char* m_szLastError;
	std::map<tsc_cookie, CSession*> m_xSessions;
	tsc_cookie m_nLastSessionCookie;
};

}; //namespace