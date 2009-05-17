#include "tscModule.h"
#include "tscSession.h"

using namespace TolonSpellCheck;
using namespace std;

static const char* const s_szErrInvalidSessionCookie =
	"Error, invalid session cookie.";
static const char* const s_szErrModuleAlreadyInitialised = 
	"Error, module object has already been initialised.";
static const char* const s_szErrModuleNotInitialised = 
	"Error, module object not initialised.";
static const char* const s_szErrNotImplemented =
	"Error, method not fully implemented.";
static const char* const s_szErrParamWasNull =
	"Error, one or more parameters were null.";
static const char* const s_szErrStructSizeInvalid =
	"Error, cbSize member of structure was set to an unrecognized value.";
static const char* const s_szSuccess =
	"Success.";
static const char* const s_szErrErr =
	"Internal error, error text not set!";

CModule* CModule::sm_pThis;

CModule* CModule::GetInstance()
{
	if (sm_pThis)
		return sm_pThis;

	try {
		sm_pThis = new CModule;
	} catch (bad_alloc& e) {
		//couldn't allocate new module
		//TODO: add logging
	}
}

CModule::CModule() :
	m_bInitialised(false),
	m_szLastError(s_szErrErr),
	m_nLastSessionCookie(TSC_NULL_COOKIE)
{
}

CModule::~CModule()
{
}

tsc_result CModule::Init(TSC_INIT_DATA* pData)
{
	if (IsInitialised())
		return Error_ModuleAlreadyInitialised();

	if (!pData)
		return Error_ParamWasNull();
	
	if (pData->cbSize != sizeof(TSC_INIT_DATA))
		return Error_StructSizeInvalid();
	
	m_sHostName = pData->szAppName;
	SetInitialised(true);
	return Success();
}

tsc_result CModule::Uninit()
{
	if (!IsInitialised())
		return Error_ModuleNotInitialised();

	SetInitialised(false);
	m_sHostName.swap(string());
	return Success();
}

tsc_result CModule::GetVersion(TSC_VERSION_DATA* pData)
{
	if (!IsInitialised())
		return Error_ModuleNotInitialised();
	
	if (!pData)
		return Error_ParamWasNull();
	
	if (pData->cbSize != sizeof(TSC_VERSION_DATA))
		return Error_StructSizeInvalid();
	
	pData->nMajor = TSC_VERSION_MAJOR;
	pData->nMinor = TSC_VERSION_MINOR;
	pData->nReserved1 = 0;
	pData->nReserved2 = 0;

	return Success();
}

tsc_result CModule::CreateSession( tsc_cookie* pSessionID, TSC_CREATESESSION_DATA* pData )
{
	if (!IsInitialised())
		return Error_ModuleNotInitialised();
	
	if (!pSessionID || !pData)
		return Error_ParamWasNull();
	
	if (pData->cbSize != sizeof(TSC_CREATESESSION_DATA))
		return Error_StructSizeInvalid();
	
	CSession* pS = new CSession(pData);
	pS->Init();
	*pSessionID = GetNextSessionCookie();
	m_xSessions.insert(std::make_pair(*pSessionID, pS));
	
	return Success();
}

tsc_result CModule::DestroySession( tsc_cookie SessionID )
{
	if (!IsInitialised())
		return Error_ModuleNotInitialised();
	
	CSession* pS = GetSession(SessionID);
	
	if (!pS)
		return Error_InvalidSessionCookie();
	
	m_xSessions.erase(SessionID);
	pS->Uninit();
	delete pS;
	
	return Success();
}

tsc_result CModule::GetSessionOptions( tsc_cookie SessionID,  TSC_SESSIONOPTIONS_DATA* pData )
{
	if (!IsInitialised())
		return Error_ModuleNotInitialised();
	
	if (!pData)
		return Error_ParamWasNull();
	
	if (pData->cbSize != sizeof(TSC_SESSIONOPTIONS_DATA))
		return Error_StructSizeInvalid();
	
	CSession* pS = GetSession(SessionID);
	
	if (!pS)
		return Error_InvalidSessionCookie();
	
	tsc_result r = TSC_E_FAIL;
	r = pS->GetSessionOptions(pData);
	m_szLastError = pS->GetLastError();
	
	return r;
}

tsc_result CModule::SetSessionOptions( tsc_cookie SessionID, TSC_SESSIONOPTIONS_DATA* pData )
{
	if (!IsInitialised())
		return Error_ModuleNotInitialised();
	
	if (!pData)
		return Error_ParamWasNull();
	
	if (pData->cbSize != sizeof(TSC_SESSIONOPTIONS_DATA))
		return Error_StructSizeInvalid();
	
	CSession* pS = GetSession(SessionID);
	
	if (!pS)
		return Error_InvalidSessionCookie();
	
	tsc_result r = TSC_E_FAIL;
	r = pS->SetSessionOptions(pData);
	m_szLastError = pS->GetLastError();
	
	return r;
}

tsc_result CModule::ShowOptionsWindow( tsc_cookie SessionID, TSC_SHOWOPTIONSWINDOW_DATA* pData )
{
	if (!IsInitialised())
		return Error_ModuleNotInitialised();
	
	if (!pData)
		return Error_ParamWasNull();
	
	if (pData->cbSize != sizeof(TSC_SHOWOPTIONSWINDOW_DATA))
		return Error_StructSizeInvalid();
	
	CSession* pS = GetSession(SessionID);
	
	if (!pS)
		return Error_InvalidSessionCookie();
	
	tsc_result r = TSC_E_FAIL;
	r = pS->ShowOptionsWindow(pData);
	m_szLastError = pS->GetLastError();
	
	return r;
}

tsc_result CModule::CheckSpelling( tsc_cookie SessionID, TSC_CHECKSPELLING_DATA* pData )
{
	if (!IsInitialised())
		return Error_ModuleNotInitialised();
	
	if (!pData)
		return Error_ParamWasNull();
	
	if (pData->cbSize != sizeof(TSC_CHECKSPELLING_DATA))
		return Error_StructSizeInvalid();
	
	CSession* pS = GetSession(SessionID);
	
	if (!pS)
		return Error_InvalidSessionCookie();
	
	tsc_result r = TSC_E_FAIL;
	r = pS->CheckSpelling(pData);
	m_szLastError = pS->GetLastError();
	
	return r;
}

tsc_result CModule::CheckWord( tsc_cookie SessionID, TSC_CHECKWORD_DATA* pData )
{
	if (!IsInitialised())
		return Error_ModuleNotInitialised();
	
	if (!pData)
		return Error_ParamWasNull();
	
	if (pData->cbSize != sizeof(TSC_CHECKWORD_DATA))
		return Error_StructSizeInvalid();
	
	CSession* pS = GetSession(SessionID);
	
	if (!pS)
		return Error_InvalidSessionCookie();
	
	tsc_result r = TSC_E_FAIL;
	r = pS->CheckWord(pData);
	m_szLastError = pS->GetLastError();
	
	return r;
}

CSession* CModule::GetSession( tsc_cookie SessionID )
{
	CSession* pS = NULL;
	std::map<tsc_cookie, CSession*>::iterator it;
	
	it = m_xSessions.find(SessionID);
	if (it != m_xSessions.end())
		pS = (*it).second;
	
	return pS;
}

tsc_cookie CModule::GetNextSessionCookie()
{
	return ++m_nLastSessionCookie;
}

// Error handlers

tsc_result CModule::Error_InvalidSessionCookie()
{
	m_szLastError = s_szErrInvalidSessionCookie;
	return TSC_E_INVALIDARG;
}

tsc_result CModule::Error_ModuleAlreadyInitialised()
{
	m_szLastError = s_szErrModuleAlreadyInitialised;
	return TSC_E_UNEXPECTED;
}

tsc_result CModule::Error_ModuleNotInitialised()
{
	m_szLastError = s_szErrModuleNotInitialised;
	return TSC_E_UNEXPECTED;
}

tsc_result CModule::Error_NotImplemented()
{
	m_szLastError = s_szErrNotImplemented;
	return TSC_E_NOTIMPL;
}

tsc_result CModule::Error_ParamWasNull()
{
	m_szLastError = s_szErrParamWasNull;
	return TSC_E_POINTER;
}

tsc_result CModule::Error_StructSizeInvalid()
{
	m_szLastError = s_szErrStructSizeInvalid;
	return TSC_E_INVALIDARG;
}

tsc_result CModule::Success()
{
	m_szLastError = s_szSuccess;
	return TSC_S_OK;
}
