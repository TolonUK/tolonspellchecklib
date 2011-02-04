#include "tscSession.h"
#include "enchant.h"
#include "tscModule.h"
#include "SpellingOptionsDlg.h"
#include <windows.h>
#include <commctrl.h>
#include <sstream>
#include "CheckSpellingDlg.h"
#include <cassert>

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
static const char* const s_szErrIntNullModulePtr = 
    "S0006 - Internal Error, a null module pointer was encountered. Please contact technical support.";
static const char* const s_szErrErr =
    "S9999 - Internal error, error text not set!";

CSession::CSession(CCreateSessionData& data) :
    m_bInitialised(false),
    m_pEnchantDict(NULL),
    m_szLastError(s_szErrErr)
{   
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

    // Provide a default culture if none has been offered.
    const bool bGotDefault = (m_options.HasDefaultLanguage() && (wcslen(m_options.DefaultLanguage()) > 0));
    if (!bGotDefault)
    {
        m_options.SetDefaultLanguageFromOS();
    }
    
    const wchar_t* szCulture = m_options.DefaultLanguage();
    if (wcslen(szCulture) > 0)
    {
        SetLanguage(szCulture);
    }
    
    SetInitialised(m_pEnchantDict != NULL);
    
    return result;
}

tsc_result CSession::Uninit()
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();
    
    tsc_result result = TSC_E_FAIL;

    SetInitialised(false);

    if (m_pEnchantDict)
    {
        CModule* pMod = CModule::GetInstance();

        if (pMod)
        {
            pMod->FreeDictionary(m_pEnchantDict);
            m_pEnchantDict = NULL;
        }
    }

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

tsc_result CSession::GetSessionOptions(CSessionOptionsData& data)
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();

    data = m_options;
    
    return Success();
}

tsc_result CSession::SetSessionOptions(CSessionOptionsData& data)
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();
    
    m_options = data;

    return Success();
}

tsc_result CSession::ShowOptionsWindow(TSC_SHOWOPTIONSWINDOW_DATA* pData)
{
    if (!IsInitialised())
        return Error_SessionNotInitialised();

    // only allow a copy of the options to be changed before we know
    // whether the dialog was dismissed by the ok button.
    CSessionOptionsData options_copy(m_options);
    
    CSpellingOptionsDlg dlg(options_copy, pData->hWndParent);
    
    if (dlg.DoModal() == IDOK)
    {
#ifdef DEBUG
        {
            std::wstringstream ss;
            ss << L"CSession::ShowOptionsWindow is saving new options." << std::endl;
            ss << L"Old options: " << m_options << std::endl;
            ss << L"New options: " << options_copy << std::endl;
            OutputDebugString(ss.str().c_str());
        }
#endif

        m_options = options_copy;

        // SET THE NEW DICTIONARY HERE!!!
        SetLanguage(m_options.CurrentLanguage());
    }
    
    return Success();
}

tsc_result CSession::SetLanguage(const wchar_t* szCulture)
{
    tsc_result result = TSC_E_FAIL;

    CModule* pMod = CModule::GetInstance();

    if (!pMod)
        return Error_Internal_NullModulePtr();

    EnchantDict* pDict = pMod->GetDictionary(szCulture);

    if (pDict)
    {
        m_pEnchantDict = pDict;
        m_szCurrentCulture.assign(szCulture);
        result = TSC_S_OK;
    }

    return result;
}

const wchar_t* CSession::GetLanguage() const
{
    const wchar_t* pResult = NULL;
    if (!m_szCurrentCulture.empty())
    {
        pResult = m_szCurrentCulture.c_str();
    }
    return pResult;
}

tsc_result CSession::Error_NotImplemented()
{
    m_szLastError = s_szErrNotImplemented;
    return TSC_E_NOTIMPL;
}

tsc_result CSession::Error_Internal_NullModulePtr()
{
    m_szLastError = s_szErrIntNullModulePtr;
    return TSC_E_FAIL;
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

////////////////////////////////////////////////////////////////////////////////
// CSessionOptions
////////////////////////////////////////////////////////////////////////////////
/*
static const wchar_t* s_szIgnoreUserDictionaries = L"IgnoreUserDictionaries";
static const wchar_t* s_szIgnoreUppercaseWords = L"IgnoreUppercaseWords";
static const wchar_t* s_szIgnoreWordsWithNumbers = L"IgnoreWordsWithNumbers";
static const wchar_t* s_szIgnoreUris = L"IgnoreUris";
static const wchar_t* s_szDictionaryCulture = L"DictionaryCulture";
static const wchar_t* s_szPreferredProvider = L"PreferredProvider";

CSessionOptions::CSessionOptions()
{
    memset(this, 0, sizeof(CSessionOptions));
}

void CSessionOptions::WriteOut()
{
    //StoreOption(s_szIgnoreUserDictionaries, bIgnoreUserDictionaries);
    //StoreOption(s_szIgnoreUppercaseWords, bIgnoreUppercaseWords);
    //StoreOption(s_szIgnoreWordsWithNumbers, bIgnoreWordsWithNumbers);
    //StoreOption(s_szIgnoreUris, bIgnoreUris);
    //StoreOption(s_szDictionaryCulture, szDictionaryCulture, sizeof(szDictionaryCulture));
    //StoreOption(s_szPreferredProvider, szPreferredProvider, sizeof(szPreferredProvider));
}

void CSessionOptions::ReadIn()
{
}

tsc_result CSessionOptions::GetCurrentLanguage(wchar_t* wszLang) const
{
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

tsc_result CSessionOptions::GetCurrentLanguage(char* szLang) const
{
    if (!szLang)
        return TSC_E_POINTER;
    
    strcpy(szLang, m_sCurrentCulture.c_str());
    return TSC_S_OK;
}

void CSessionOptions::SetLanguage(const char* szCulture)
{
    if (szCulture)
    {
        m_sCurrentCulture = szCulture;
    }
    else
    {
        m_sCurrentCulture.clear();
    }
}

void CSessionOptions::SetDefaultLanguage(const char* szLang)
{
    assert(szLang);

    if (strlen(szLang) < sizeof(szDictionaryCulture) - 1)
    {
        strcpy(szDictionaryCulture, szLang);
    }
    else
    {
        assert(false);
    }
}*/
