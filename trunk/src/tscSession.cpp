#include "tscSession.h"
#include "enchant.h"
#include "SpellingOptionsDlg.h"
#include <windows.h>
#include <commctrl.h>
#include <sstream>
#include "CheckSpellingDlg.h"
#include "isoLang.h"

using namespace std;
using namespace TolonSpellCheck;

static const char* const s_szSuccess =
    "S0000 - Success.";
static const char* const s_szErrSessionAlreadyInitialised = 
    "S0001 - Error, session object has already been initialised.";
static const char* const s_szErrSessionNotInitialised = 
    "S0002 - Error, session object not initialised.";
static const char* const s_szErrNotImplemented =
    "S0003 - Error, method not fully implemented.";
static const char* const s_szErrParamWasNull =
    "S0004 - Error, one or more parameters were null.";
static const char* const s_szErrStructSizeInvalid =
    "S0005 - Error, cbSize member of structure was set to an unrecognized value.";
static const char* const s_szErrErr =
    "S9999 - Internal error, error text not set!";

CSession::CSession(TSC_CREATESESSION_DATA* pData) :
    m_bInitialised(false),
    m_pEnchantBroker(NULL),
    m_pEnchantDict(NULL),
    m_szLastError(s_szErrErr)
{
    memset(&m_options, 0, sizeof(TSC_SESSIONOPTIONS_DATA));
    
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);
}

CSession::~CSession()
{
    
}

tsc_result CSession::Init()
{
    if (IsInitialised())
        return Error_SessionAlreadyInitialised();
    
    tsc_result result = TSC_E_FAIL;
    
    // Provide a default culture if not has been offered.
    if (strlen(m_options.szDictionaryCulture) == 0)
    {
        // win98 and later
        int n = 0;
        const int LOCALE_BUFLEN = 20;
        wchar_t wszBuf[LOCALE_BUFLEN];
        
        n = ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, wszBuf, 10);
        if (n != 0)
        {
            wcscat(wszBuf, L"-");
            n = ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, wszBuf + wcslen(wszBuf), 10);
            ::WideCharToMultiByte(CP_UTF8, 0, wszBuf, -1, m_options.szDictionaryCulture, sizeof(m_options.szDictionaryCulture), NULL, NULL);
        }
    }
        
    // Initialise the enchant library
    m_pEnchantBroker = enchant_broker_init();

    SetLanguage(m_options.szDictionaryCulture);
    
    SetInitialised(m_pEnchantDict != NULL);
    
    return result;
}

tsc_result CSession::Uninit()
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();
    
    tsc_result result = TSC_E_FAIL;

    if (m_pEnchantBroker)
    {
        if (m_pEnchantDict)
        {
            enchant_broker_free_dict (m_pEnchantBroker, m_pEnchantDict);
            m_pEnchantDict = NULL;
        }
        m_pEnchantBroker = NULL;
    }
    SetInitialised(false);
    result = TSC_S_OK;

    return result;
}

tsc_result CSession::CheckSpelling(TSC_CHECKSPELLING_DATA* pData)
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();

    if (!pData)
        return Error_ParamWasNull();
    
    if (pData->cbSize != sizeof(TSC_CHECKSPELLING_DATA))
        return Error_StructSizeInvalid();
    
    if ( (pData->nMode == MODE_AUTO) )
    {
        CCheckSpellingDlg dlg(this, pData);
        
        dlg.DoModal();
    }
    
    return Success();
}

tsc_result CSession::CheckWord(TSC_CHECKWORD_DATA* pData)
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();

    if (!pData)
        return Error_ParamWasNull();
    
    if (pData->cbSize != sizeof(TSC_CHECKWORD_DATA))
        return Error_StructSizeInvalid();
    
    tsc_result tr = TSC_E_UNEXPECTED;
    
    if ( enchant_dict_check(m_pEnchantDict, pData->uTestWord.szWord8, -1) == 0 )
    {
        // word found
        tr = TSC_S_OK;
        pData->bOk = true;
    }
    else
    {
        // can't find word, get suggestions
        tr = TSC_S_FALSE;
        pData->bOk = false;

        char **suggs = NULL;
        size_t n_suggs = 0;

        suggs = enchant_dict_suggest( m_pEnchantDict,
                                      pData->uTestWord.szWord8,
                                      strlen(pData->uTestWord.szWord8),
                                      &n_suggs );

        if ((pData->uResultString.szResults8) && (pData->nResultStringSize))
        {
            pData->uResultString.szResults8[0] = 0;
            pData->uResultString.szResults8[1] = 0;
        }

    if (suggs && n_suggs)
        {
            // provide list of words separated by null characters.
            char* szOut = pData->uResultString.szResults8;
            
            if (szOut)
            {
                char* szSug = NULL;
                size_t nOutDone = 0;
                size_t nSugLen = 0;
                for ( size_t i = 0; i < n_suggs; ++i )
                {
                    szSug = suggs[i];

                    if (szSug)
                    {
                        nSugLen = strlen(szSug) + 1;

                        // The +1 is to make sure we leave enough space for the double null terminator
                        if ( (nOutDone + nSugLen + 1) < pData->nResultStringSize )
                        {
                            memcpy(&szOut[nOutDone], szSug, nSugLen);
                            nOutDone += nSugLen;
                        }
                    }
                }

                szOut[nOutDone] = '\0'; // This gives a double null-terminator at the end.
            }

            enchant_dict_free_string_list(m_pEnchantDict, suggs);
        }
    }

    if (FAILED(tr))
        return tr;
    
    pData->bOk = (tr == TSC_S_OK);
    
    return Success();
}

tsc_result CSession::GetSessionOptions(TSC_SESSIONOPTIONS_DATA* pData)
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();

    if (!pData)
        return Error_ParamWasNull();
    
    if (pData->cbSize != sizeof(TSC_SESSIONOPTIONS_DATA))
        return Error_StructSizeInvalid();
    
    memcpy(pData, &m_options, sizeof(TSC_SESSIONOPTIONS_DATA));
    
    return Success();
}

tsc_result CSession::SetSessionOptions(TSC_SESSIONOPTIONS_DATA* pData)
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();

    if (!pData)
        return Error_ParamWasNull();
    
    if (pData->cbSize != sizeof(TSC_SESSIONOPTIONS_DATA))
        return Error_StructSizeInvalid();
    
    return Error_NotImplemented();
}

tsc_result CSession::ShowOptionsWindow(TSC_SHOWOPTIONSWINDOW_DATA* pData)
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();
    
    CSpellingOptionsDlg dlg(this, pData->hWndParent);
    
    dlg.DoModal();
    
    return Success();
}

void CSession::cbEnchantDictDescribe( const char * const lang_tag,
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

tsc_result CSession::GetCurrentLanguage(wchar_t* wszLang) 
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();

    if (!wszLang)
        return Error_ParamWasNull();
    
    tsc_result result = TSC_E_FAIL;
    int n = 0;
//	tsc_sizet nwszLen = _wcslen(wszLang);
    
//	if (nwszLen > 12)
//		return TSC_E_INVALIDARG; // lang descriptor was too long
    
    char szLang[13];
    result = GetCurrentLanguage(szLang);
    if (SUCCEEDED(result))
    {
        n = ::MultiByteToWideChar(CP_UTF8, 0, szLang, -1, wszLang, 13);
        if (!n)
            result = TSC_E_FAIL;
    }
    
    return result;
}

tsc_result CSession::GetCurrentLanguage(char* szLang) 
{
    if (!IsInitialised())
        return TSC_E_UNEXPECTED;

    if (!szLang)
        return TSC_E_POINTER;
    
    strcpy(szLang, m_szCurrentCulture.c_str());
    return TSC_S_OK;
}

tsc_result CSession::DescribeLanguage(const wchar_t* wszLang, LANGUAGE_DESC_WIDEDATA* pWideData) 
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();

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

tsc_result CSession::DescribeLanguage(const char* szLang, LANGUAGE_DESC_DATA* pData) 
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();

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

tsc_result CSession::EnumLanguages(LanguageEnumFn pfn, void* pUserData)
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();

    if (!pfn)
        return Error_ParamWasNull();
    
    EnumLanguagesPayload elp = {0};
    elp.pfn = pfn;
    elp.pUserData = pUserData;
    elp.pThis = this;
    enchant_broker_list_dicts (m_pEnchantBroker, CSession::cbEnchantDictDescribe, &elp);
    
    return Success();
}

tsc_result CSession::SetLanguage(const char* szCulture)
{
    tsc_result result = TSC_E_FAIL;

    if (m_pEnchantBroker)
    {
        EnchantDict* pDict = NULL;
        pDict = enchant_broker_request_dict(m_pEnchantBroker, szCulture);

        if (pDict)
        {
            m_pEnchantDict = pDict;
            m_szCurrentCulture.assign(szCulture);
            result = TSC_S_OK;
        }
    }

    return result;
}

tsc_result CSession::Error_NotImplemented()
{
    m_szLastError = s_szErrNotImplemented;
    return TSC_E_NOTIMPL;
}

tsc_result CSession::Error_ParamWasNull()
{
    m_szLastError = s_szErrParamWasNull;
    return TSC_E_POINTER;
}

tsc_result CSession::Error_SessionAlreadyInitialised()
{
    m_szLastError = s_szErrSessionAlreadyInitialised;
    return TSC_E_UNEXPECTED;
}

tsc_result CSession::Error_SessionNotInitialised()
{
    m_szLastError = s_szErrSessionNotInitialised;
    return TSC_E_UNEXPECTED;
}

tsc_result CSession::Error_StructSizeInvalid()
{
    m_szLastError = s_szErrStructSizeInvalid;
    return TSC_E_INVALIDARG;
}

tsc_result CSession::Success()
{
    m_szLastError = s_szSuccess;
    return TSC_S_OK;
}
