#include "tscModule.h"
#include "tscSession.h"
#include <windows.h>
#include "isoLang.h"
#include <sstream>

using namespace TolonSpellCheck;
using namespace std;

static const char* const s_szSuccess =
	"M0000 - Success.";
static const char* const s_szErrInvalidSessionCookie =
	"M0001 - Error, invalid session cookie.";
static const char* const s_szErrModuleAlreadyInitialised = 
	"M0002 - Error, module object has already been initialised.";
static const char* const s_szErrModuleNotInitialised = 
	"M0003 - Error, module object not initialised.";
static const char* const s_szErrNotImplemented =
	"M0004 - Error, method not fully implemented.";
static const char* const s_szErrParamWasNull =
	"M0005 - Error, one or more parameters were null.";
static const char* const s_szErrStructSizeInvalid =
	"M0006 - Error, cbSize member of structure was set to an unrecognized value.";
static const char* const s_szErrSessionCreationFailed =
    "M0008 - Error, session creation failed.";
static const char* const s_szErrFailedToInitEnchant =
    "M0009 - Error, failed to initialise the spell checking engine.";
static const char* const s_szErrErr =
	"M9999 - Internal error, error text not set!";

CModule* CModule::sm_pThis;

CModule* CModule::GetInstance()
{
    //TODO: Do we need concurrency protection here?

	if (sm_pThis)
		return sm_pThis;

	try {
		sm_pThis = new CModule;
	} catch (bad_alloc& e) {
		//couldn't allocate new module
		//TODO: add logging
	}

    return sm_pThis;
}

CModule::CModule() :
	m_bInitialised(false),
	m_szLastError(s_szErrErr),
	m_nLastSessionCookie(TSC_NULL_COOKIE),
    m_pEnchantBroker(NULL)
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

    // Initialise the enchant library
    m_pEnchantBroker = enchant_broker_init();

	SetInitialised(m_pEnchantBroker != NULL);

    return m_pEnchantBroker ? Success() : Error_FailedToInitEnchant();
}

tsc_result CModule::Uninit()
{
	if (!IsInitialised())
		return Error_ModuleNotInitialised();

	SetInitialised(false);

    if (m_pEnchantBroker)
    {
        enchant_broker_free(m_pEnchantBroker);
        m_pEnchantBroker = NULL;
    }

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
	
    bool bSessionOk = false;
    CSession* pS = NULL;
    try { pS = new CSession(pData); } catch (bad_alloc& e) { }

    if (pS)
    {
	    pS->Init();
        bSessionOk = pS->IsInitialised();
    }

    if (bSessionOk)
    {
	    *pSessionID = GetNextSessionCookie();
	    m_xSessions.insert(std::make_pair(*pSessionID, pS));
	    return Success();
    }
    else
    {
        delete pS;
        return Error_SessionCreationFailed();
    }
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

EnchantDict* CModule::GetDictionary(const char* szCulture)
{
    EnchantDict* pDict = NULL;

    if (m_pEnchantBroker)
    {
        pDict = enchant_broker_request_dict(m_pEnchantBroker, szCulture);
    }

    return pDict;
}

void CModule::FreeDictionary(EnchantDict* pDict)
{
    if (m_pEnchantBroker)
    {
        enchant_broker_free_dict(m_pEnchantBroker, pDict);
    }
}

// Language Enumeration
tsc_result CModule::EnumLanguages(LanguageEnumFn pfn, void* pUserData)
{
    if (!IsInitialised())
        return Error_ModuleNotInitialised();

    if (!pfn)
        return Error_ParamWasNull();
    
    EnumLanguagesPayload elp = {0};
    elp.pfn = pfn;
    elp.pUserData = pUserData;
    elp.pThis = this;
    enchant_broker_list_dicts (m_pEnchantBroker, CModule::cbEnchantDictDescribe, &elp);
    
    return Success();
}

void CModule::cbEnchantDictDescribe( const char * const lang_tag,
                                     const char * const provider_name,
                                     const char * const provider_desc,
                                     const char * const provider_file,
                                     void * user_data )
{
    EnumLanguagesPayload* pelp = reinterpret_cast<EnumLanguagesPayload*>(user_data);
    
    if (!pelp)
        return;

    wchar_t wszLangTag[64];
    
    wszLangTag[0] = 0;
    
    ::MultiByteToWideChar(CP_UTF8, 0, lang_tag, -1, wszLangTag, 64);
    
    LANGUAGE_DESC_WIDEDATA ldwd = {0};
    ldwd.cbSize = sizeof(LANGUAGE_DESC_WIDEDATA);

    pelp->pThis->DescribeLanguage(wszLangTag, &ldwd);
    
    wcscpy(ldwd.wszCodeName, wszLangTag);
    pelp->pfn(&ldwd, pelp->pUserData);
}

tsc_result CModule::DescribeLanguage(const wchar_t* wszLang, LANGUAGE_DESC_WIDEDATA* pWideData) 
{
    if (!IsInitialised())
        return Error_ModuleNotInitialised();

    if (!wszLang || !pWideData)
        return Error_ParamWasNull();
    
    if (pWideData->cbSize != sizeof(LANGUAGE_DESC_WIDEDATA))
        return Error_StructSizeInvalid();

    tsc_result result = TSC_E_FAIL;
    int n = 0;
    tsc_size_t nwszLen = wcslen(wszLang);
    
    if (nwszLen > 12)
        return TSC_E_INVALIDARG; // lang descriptor was too long
    
    char szLang[13];
    memset(szLang, 0, sizeof(szLang));
    n = ::WideCharToMultiByte(CP_UTF8, 0, wszLang, nwszLen, szLang, 13, NULL, NULL);
    
    if (n)
    {
        LANGUAGE_DESC_DATA data;
        memset(&data, 0, sizeof(LANGUAGE_DESC_DATA));
        data.cbSize = sizeof(LANGUAGE_DESC_DATA);
        result = DescribeLanguage(szLang, &data);
        
        if (SUCCEEDED(result))
        {
            n = ::MultiByteToWideChar(CP_UTF8, 0, data.szDisplayName, -1, pWideData->wszDisplayName, 128);
            if (!n)
                result = TSC_E_FAIL;
        }
    }
    
    return result;
}

tsc_result CModule::DescribeLanguage(const char* szLang, LANGUAGE_DESC_DATA* pData) 
{
    if (!IsInitialised())
        return Error_ModuleNotInitialised();

    if (!szLang || !pData)
        return Error_ParamWasNull();
    
    if (pData->cbSize != sizeof(LANGUAGE_DESC_DATA))
        return Error_StructSizeInvalid();
    
    tsc_result result = TSC_E_FAIL;
    
    CIsoLang il;
    std::string sLanguage;
    std::string sRegion;
    std::string sDesc;
    std::stringstream ss;
    
    il.Parse(szLang, sLanguage, sRegion);
    
    if (sLanguage.empty() == false)
    {
        ss << sLanguage; 
        
        if (sRegion.empty() == false)
        {
            ss << ", " << sRegion;
        }
        
        sDesc.swap(ss.str());
        memcpy(pData->szDisplayName, sDesc.c_str(), min(sDesc.size(), 127));
        pData->szDisplayName[127] = '\0';

        result = TSC_S_OK;
    }
    
    return result;
}

// Error handlers
tsc_result CModule::Error_FailedToInitEnchant()
{
    m_szLastError = s_szErrFailedToInitEnchant;
    return TSC_E_UNEXPECTED;
}

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

tsc_result CModule::Error_SessionCreationFailed()
{
    m_szLastError = s_szErrSessionCreationFailed;
    return TSC_E_UNEXPECTED;
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
