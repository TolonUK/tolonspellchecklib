#include "TolonSpellCheck.h"
#include "tscModule.h"
#include "tscSession.h"

using namespace TolonSpellCheck;

tsc_result TSC_CALLTYPE
	tscInit( TSC_INIT_DATA* pData )
{
	tsc_result r = TSC_E_FAIL;
	CModule* pM = CModule::GetInstance();

	if (!pData)
	{
		r = TSC_E_INVALIDARG;
		//TODO: useful error message required
	}
	else
	{
		if (pM)
			r = pM->Init(pData->szAppName);
		else
		{
			r = TSC_E_FAIL;
			//TODO: useful error message here
		}
	}
	
	return r;
}

tsc_result TSC_CALLTYPE
	tscUninit( void )
{
	tsc_result r = TSC_E_FAIL;
	CModule* pM = CModule::GetInstance();

	if (!pM->IsInitialised())
	{
		r = TSC_E_UNEXPECTED;
		//TODO: useful error message here
	}
	else
		r = pM->Uninit();

	return r;
}

tsc_result TSC_CALLTYPE
	tscGetVersion( TSC_VERSION_DATA* pData )
{
    return E_NOTIMPL;
}

tsc_result TSC_CALLTYPE 
	tscCreateSession( tsc_cookie* pSessionID,
					  TSC_CREATESESSION_DATA* pData )
{
	return E_NOTIMPL;
}

tsc_result TSC_CALLTYPE 
	tscDestroySession( tsc_cookie SessionID )
{
	return E_NOTIMPL;
}

tsc_result TSC_CALLTYPE 
	tscGetSessionOptions( tsc_cookie SessionID, 
                          TSC_SESSIONOPTIONS_DATA* pData )
{
	return E_NOTIMPL;
}

tsc_result TSC_CALLTYPE 
	tscSetSessionOptions( tsc_cookie SessionID,
                          TSC_SESSIONOPTIONS_DATA* pData )
{
	return E_NOTIMPL;
}

tsc_result TSC_CALLTYPE 
	tscShowOptionsWindow( tsc_cookie SessionID,
                          TSC_SHOWOPTIONSWINDOW_DATA* pData )
{
    tsc_result result = TSC_E_FAIL;
    
    CSession s;
    s.Init();
    result = s.ShowOptionsWindow();
    s.Uninit();

    return result;
}

tsc_result TSC_CALLTYPE 
	tscCheckSpelling( tsc_cookie SessionID,
                      TSC_CHECKSPELLING_DATA* pData )
{
	return E_NOTIMPL;
}

tsc_result TSC_CALLTYPE
	tscCheckWord( tsc_cookie SessionID,
				  TSC_CHECKWORD_DATA* pData )
{
	tsc_result result = TSC_E_FAIL;

	if (!pData)
		return TSC_E_INVALIDARG;

	CSession s;
	s.Init();
	result = s.CheckWord(pData->uTestWord.szWord8);
	s.Uninit();

	return result;
}