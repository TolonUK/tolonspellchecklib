#include "TolonSpellCheck.h"
#include "tscModule.h"
#include "tscSession.h"

using namespace TolonSpellCheck;

HINSTANCE g_hInstDll;

// Error-handling Utils

static const char* s_szLastError;

// Error strings (to be localised)
static const char* const s_szErrIntNullModulePtr = 
	"Internal Error, a null module pointer was encountered. Please contact technical support.";
static const char* const s_szErrParamWasNull =
	"Error, one or more parameters were null.";
	
/*static void Error_GetLastModuleError(CModule* pM)
{
	s_szLastError = pM->GetLastError();
}*/

static tsc_result Error_Internal_NullModulePtr()
{
	s_szLastError = s_szErrIntNullModulePtr;
	return TSC_E_FAIL;
}

static tsc_result Error_ParamWasNull()
{
	s_szLastError = s_szErrParamWasNull;
	return TSC_E_FAIL;
}

// The exported DLL functions

tsc_result TSC_CALLTYPE
	tscInit( TSC_INIT_DATA* pData )
{
	CModule* pM = CModule::GetInstance();
	if (!pM)
		return Error_Internal_NullModulePtr();

	tsc_result r = TSC_E_FAIL;
	r = pM->Init(pData);
	//if (TSC_FAILED(r))
		//Error_GetLastModuleError(pM);
	return r;
}

tsc_result TSC_CALLTYPE
	tscUninit( void )
{
	CModule* pM = CModule::GetInstance();
	if (!pM)
		return Error_Internal_NullModulePtr();

	tsc_result r = TSC_E_FAIL;
	r = pM->Uninit();
	//if (TSC_FAILED(r))
		//Error_GetLastModuleError(pM);
	return r;
}

tsc_result TSC_CALLTYPE
	tscGetVersion( TSC_VERSION_DATA* pData )
{
	CModule* pM = CModule::GetInstance();
	if (!pM)
		return Error_Internal_NullModulePtr();

	tsc_result r = TSC_E_FAIL;
	r = pM->GetVersion(pData);
	//if (TSC_FAILED(r))
		//Error_GetLastModuleError(pM);
	return r;
}

/*tsc_result TSC_CALLTYPE
	tscGetLastError( const char ** ppszError )
{
	if (!ppszError)
		return Error_ParamWasNull();
	
	if (s_szLastError)
	{
		*ppszError = s_szLastError;
		return TSC_S_OK;
	}
	else
	{
		*ppszError = NULL;
		return TSC_S_FALSE;
	}
}*/

const char* TSC_CALLTYPE
	tscGetLastError()
{
	CModule* pM = CModule::GetInstance();
	if (!pM)
		return s_szLastError;
	else
		return pM->GetLastError();
}


tsc_result TSC_CALLTYPE 
	tscCreateSession( tsc_cookie* pSessionID,
					  TSC_CREATESESSION_DATA* pData )
{
	CModule* pM = CModule::GetInstance();
	if (!pM)
		return Error_Internal_NullModulePtr();

	tsc_result r = TSC_E_FAIL;

    if (pSessionID)
    {
	    r = pM->CreateSession(pSessionID, pData);

	    //if (TSC_FAILED(r))
		    //Error_GetLastModuleError(pM);
    }

	return r;
}

tsc_result TSC_CALLTYPE 
	tscDestroySession( tsc_cookie SessionID )
{
	CModule* pM = CModule::GetInstance();
	if (!pM)
		return Error_Internal_NullModulePtr();

	tsc_result r = TSC_E_FAIL;
	r = pM->DestroySession(SessionID);
	//if (TSC_FAILED(r))
		//Error_GetLastModuleError(pM);
	return r;
}

tsc_result TSC_CALLTYPE 
	tscGetSessionOptions( tsc_cookie SessionID, 
						  TSC_SESSIONOPTIONS_DATA* pData )
{
	CModule* pM = CModule::GetInstance();
	if (!pM)
		return Error_Internal_NullModulePtr();

	tsc_result r = TSC_E_FAIL;
	r = pM->GetSessionOptions(SessionID, pData);
	//if (TSC_FAILED(r))
		//Error_GetLastModuleError(pM);
	return r;
}

tsc_result TSC_CALLTYPE 
	tscSetSessionOptions( tsc_cookie SessionID,
						  TSC_SESSIONOPTIONS_DATA* pData )
{
	CModule* pM = CModule::GetInstance();
	if (!pM)
		return Error_Internal_NullModulePtr();

	tsc_result r = TSC_E_FAIL;
	r = pM->SetSessionOptions(SessionID, pData);
	//if (TSC_FAILED(r))
		//Error_GetLastModuleError(pM);
	return r;
}

tsc_result TSC_CALLTYPE 
	tscShowOptionsWindow( tsc_cookie SessionID,
						  TSC_SHOWOPTIONSWINDOW_DATA* pData )
{    
	CModule* pM = CModule::GetInstance();
	if (!pM)
		return Error_Internal_NullModulePtr();

	tsc_result r = TSC_E_FAIL;
	r = pM->ShowOptionsWindow(SessionID, pData);
	//if (TSC_FAILED(r))
		//Error_GetLastModuleError(pM);
	return r;
}

tsc_result TSC_CALLTYPE 
	tscCheckSpelling( tsc_cookie SessionID,
					  TSC_CHECKSPELLING_DATA* pData )
{
	CModule* pM = CModule::GetInstance();
	if (!pM)
		return Error_Internal_NullModulePtr();

	tsc_result r = TSC_E_FAIL;
	r = pM->CheckSpelling(SessionID, pData);
	//if (TSC_FAILED(r))
		//Error_GetLastModuleError(pM);
	return r;
}

tsc_result TSC_CALLTYPE
	tscCheckWord( tsc_cookie SessionID,
				  TSC_CHECKWORD_DATA* pData )
{
	CModule* pM = CModule::GetInstance();
	if (!pM)
		return Error_Internal_NullModulePtr();

	tsc_result r = TSC_E_FAIL;
	r = pM->CheckWord(SessionID, pData);
	//if (TSC_FAILED(r))
		//Error_GetLastModuleError(pM);
	return r;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL,  // handle to the DLL module
                     DWORD fdwReason,     // reason for calling function
                     LPVOID lpvReserved ) // reserved
{
    g_hInstDll = hinstDLL;
    return TRUE;
}
