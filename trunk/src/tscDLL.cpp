#include "TolonSpellCheck.h"
#include "TolonSpellCheckInternals.h"
#include "tscModule.h"
#include "tscSession.h"
#include <windows.h>
#include <commctrl.h>

using namespace TolonSpellCheck;

HINSTANCE g_hInstDll;

// Error-handling Utils

static const char* s_szLastError;

// Error strings (to be localised)
static const char* const s_szErrNoErr = "";
static const char* const s_szErrIntNullModulePtr = 
    "D0001 - Internal Error, a null module pointer was encountered. Please contact technical support.";
static const char* const s_szErrParamWasNull =
    "D0002 - Error, one or more parameters were null.";
static const char* const s_szErrStructSizeInvalid =
    "D0003 - Error, cbSize member of structure was set to an unrecognized value.";
static const char* const s_szErrErr =
    "D9999 - Internal error, error text not set!";
    
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

static tsc_result Error_StructSizeInvalid()
{
    s_szLastError = s_szErrStructSizeInvalid;
    return TSC_E_FAIL;
}

// Private functions

void tsc_func_init()
{
    s_szLastError = s_szErrNoErr;

    CModule* pM = CModule::GetInstance();
    if (pM)
        pM->ClearLastError();
}

// The exported DLL functions

tsc_result TSC_CALLTYPE
    tscInit( TSC_INIT_DATA* pData )
{
    tsc_func_init();

    CModule* pM = CModule::GetInstance(g_hInstDll);
    if (!pM)
        return Error_Internal_NullModulePtr();
    if (!pData)
        return Error_ParamWasNull();
    if (pData->cbSize != sizeof(TSC_INIT_DATA))
        return Error_StructSizeInvalid();

    CInitData d(*pData);

    tsc_result r = TSC_E_FAIL;
    r = pM->Init(d);

    return r;
}

tsc_result TSC_CALLTYPE
    tscUninit( void )
{
    tsc_func_init();

    CModule* pM = CModule::GetInstance();
    if (!pM)
        return Error_Internal_NullModulePtr();

    tsc_result r = TSC_E_FAIL;
    r = pM->Uninit();

    return r;
}

tsc_result TSC_CALLTYPE
    tscGetVersion( TSC_VERSION_DATA* pData )
{
    tsc_func_init();

    CModule* pM = CModule::GetInstance();
    if (!pM)
        return Error_Internal_NullModulePtr();
    if (!pData)
        return Error_ParamWasNull();
    if (pData->cbSize != sizeof(TSC_VERSION_DATA))
        return Error_StructSizeInvalid();

    CVersionData d(*pData);

    tsc_result r = TSC_E_FAIL;
    r = pM->GetVersion(d);

    if (TSC_SUCCEEDED(r))
    {
        *pData << d;
    }

    return r;
}

const char* TSC_CALLTYPE
    tscGetLastError()
{
    if (s_szLastError != s_szErrNoErr)
        return s_szLastError;
    else
    {
        CModule* pM = CModule::GetInstance();
        if (pM)
            return pM->GetLastError();
        else
            return s_szErrErr; // catch-all error text
    }
}


tsc_result TSC_CALLTYPE 
    tscCreateSession( tsc_cookie* pSessionID,
                      TSC_CREATESESSION_DATA* pData )
{
    tsc_func_init();

    CModule* pM = CModule::GetInstance();
    if (!pM)
        return Error_Internal_NullModulePtr();
    if (!pSessionID || !pData)
        return Error_ParamWasNull();
    if (pData->cbSize != sizeof(TSC_CREATESESSION_DATA))
        return Error_StructSizeInvalid();

    CCreateSessionData d(*pData);

    tsc_result r = TSC_E_FAIL;

    r = pM->CreateSession(pSessionID, d);

    return r;
}

tsc_result TSC_CALLTYPE 
    tscDestroySession( tsc_cookie SessionID )
{
    tsc_func_init();

    CModule* pM = CModule::GetInstance();
    if (!pM)
        return Error_Internal_NullModulePtr();

    tsc_result r = TSC_E_FAIL;
    r = pM->DestroySession(SessionID);

    return r;
}

tsc_result TSC_CALLTYPE 
    tscGetSessionOptions( tsc_cookie SessionID, 
                          TSC_SESSIONOPTIONS_DATA* pData )
{
    tsc_func_init();

    CModule* pM = CModule::GetInstance();
    if (!pM)
        return Error_Internal_NullModulePtr();
    if (!pData)
        return Error_ParamWasNull();
    if (pData->cbSize != sizeof(TSC_SESSIONOPTIONS_DATA))
        return Error_StructSizeInvalid();

    CSessionOptionsData d;
    tsc_result r = TSC_E_FAIL;
    r = pM->GetSessionOptions(SessionID, d);
    *pData << d;

    return r;
}

tsc_result TSC_CALLTYPE 
    tscSetSessionOptions( tsc_cookie SessionID,
                          TSC_SESSIONOPTIONS_DATA* pData )
{
    tsc_func_init();

    CModule* pM = CModule::GetInstance();
    if (!pM)
        return Error_Internal_NullModulePtr();
    if (!pData)
        return Error_ParamWasNull();
    if (pData->cbSize != sizeof(TSC_SESSIONOPTIONS_DATA))
        return Error_StructSizeInvalid();

    CSessionOptionsData d(*pData);
    tsc_result r = TSC_E_FAIL;
    r = pM->SetSessionOptions(SessionID, d);

    return r;
}

tsc_result TSC_CALLTYPE 
    tscShowOptionsWindow( tsc_cookie SessionID,
                          TSC_SHOWOPTIONSWINDOW_DATA* pData )
{
    tsc_func_init();

    CModule* pM = CModule::GetInstance();
    if (!pM)
        return Error_Internal_NullModulePtr();
    if (!pData)
        return Error_ParamWasNull();

    tsc_result r = TSC_E_FAIL;
    r = pM->ShowOptionsWindow(SessionID, pData);

    return r;
}

tsc_result TSC_CALLTYPE 
    tscCheckSpelling( tsc_cookie SessionID,
                      TSC_CHECKSPELLING_DATA* pData )
{
    tsc_func_init();

    CModule* pM = CModule::GetInstance();
    if (!pM)
        return Error_Internal_NullModulePtr();
    if (!pData)
        return Error_ParamWasNull();

    tsc_result r = TSC_E_FAIL;
    r = pM->CheckSpelling(SessionID, pData);

    return r;
}

tsc_result TSC_CALLTYPE
    tscCheckWord( tsc_cookie SessionID,
                  TSC_CHECKWORD_DATA* pData )
{
    tsc_func_init();

    CModule* pM = CModule::GetInstance();
    if (!pM)
        return Error_Internal_NullModulePtr();
    if (!pData)
        return Error_ParamWasNull();

    tsc_result r = TSC_E_FAIL;
    r = pM->CheckWord(SessionID, pData);

    return r;
}

tsc_result TSC_CALLTYPE
	tscCustomDic( tsc_cookie SessionID,
				  TSC_CUSTOMDIC_DATA* pData )
{
	tsc_func_init();

    CModule* pM = CModule::GetInstance();
    if (!pM)
        return Error_Internal_NullModulePtr();
    if (!pData)
        return Error_ParamWasNull();

    tsc_result r = TSC_E_FAIL;
    r = pM->CustomDic(SessionID, pData);

    return r;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL,  // handle to the DLL module
                     DWORD /*fdwReason*/,     // reason for calling function
                     LPVOID /*lpvReserved*/ ) // reserved
{
    g_hInstDll = hinstDLL;
    ::InitCommonControls();
    return TRUE;
}
