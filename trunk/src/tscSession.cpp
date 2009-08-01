#include "tscSession.h"
#include "enchant.h"
#include "SpellingOptionsDlg.h"
#include <windows.h>
#include <commctrl.h>
#include <sstream>
#include "CheckSpellingDlg.h"

using namespace std;
using namespace TolonSpellCheck;

static const char* const s_szErrSessionAlreadyInitialised = 
	"Error, session object has already been initialised.";
static const char* const s_szErrSessionNotInitialised = 
	"Error, session object not initialised.";
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

CSession::CSession(TSC_CREATESESSION_DATA* pData) :
	m_bInitialised(false),
	m_pEnchantBroker(NULL),
	m_szLastError(s_szErrErr)
{
	memset(&m_options, 0, sizeof(TSC_SESSIONOPTIONS_DATA));
	/*
	if (pData && (pData->cbSize <= sizeof(TSC_CREATESESSION_DATA)))
	{
		memset(&m_data, 0, sizeof(TSC_CREATESESSION_DATA));
		memcpy(&m_data, pData, pData->cbSize);
	}
	*/
	
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

	if (m_pEnchantBroker)
	{
		m_pEnchantDict = enchant_broker_request_dict(m_pEnchantBroker, m_options.szDictionaryCulture);

		if (m_pEnchantDict)
		{
			result = TSC_S_OK;
			//SetInitialised(true);
		}
		else
		{
			enchant_broker_free(m_pEnchantBroker);
			m_pEnchantBroker = NULL;
		}
	}
	
	SetInitialised(true);

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


	/*if ( (pData->nMode == MODE_AUTO) && (pData->nTarget == TARGET_RICHEDIT) )
	{
		CRichEditSpellChecker r(this);
		
		r.CheckSpelling(pData->hwnd);
	}*/
	
	return Success();
}

tsc_result CSession::CheckWord(const char* szWord)
{
	tsc_result result = TSC_E_FAIL;

	if (!IsInitialised())
		return Error_SessionNotInitialised();

	if (!szWord)
		return Error_ParamWasNull();

	if ( enchant_dict_check(m_pEnchantDict, szWord, -1) == 0 )
	{
		// word found
		result = TSC_S_OK;
	}
	else
	{
		// can't find word, get suggestions
		result = TSC_S_FALSE;

		/*char **suggs = NULL;
		size_t n_suggs = 0;

		suggs = enchant_dict_suggest( m_pEnchantDict,
									  szWord,
									  strlen(szWord),
									  &n_suggs );

		if (suggs && n_suggs)
			enchant_dict_free_string_list(m_pEnchantDict, suggs);*/
	}

	return result;
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
	
	tr = CheckWord(pData->uTestWord.szWord8);
	
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
	
	//enchant_broker_list_dicts (m_pEnchantBroker, CSession::cbEnchantDictDescribe, NULL);
	
	return Success();
}

void CSession::cbEnchantDictDescribe( const char * const lang_tag,
									   const char * const provider_name,
									   const char * const provider_desc,
									   const char * const provider_file,
									   void * user_data )
{
	wchar_t wszLangTag[64];
	wchar_t wszProvName[256];
	wchar_t wszProvDesc[256];
	
	wszLangTag[0] = 0;
	wszProvName[0] = 0;
	wszProvDesc[0] = 0;
	
	::MultiByteToWideChar(CP_UTF8, 0, lang_tag, -1, wszLangTag, 64);
	::MultiByteToWideChar(CP_UTF8, 0, provider_name, -1, wszProvName, 256);
	::MultiByteToWideChar(CP_UTF8, 0, provider_desc, -1, wszProvDesc, 256);
	
	wstringstream ss;
	
	ss << wszLangTag << wszProvName << wszProvDesc;
	
	MessageBox(NULL, ss.str().c_str(), NULL, MB_OK);
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
	
	strcpy(szLang, m_options.szDictionaryCulture);
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

	if (_stricmp(szLang, "en-gb") == 0)
	{
		strcpy(pData->szDisplayName, "English, United Kingdom (en-gb)");
		result = TSC_S_OK;
	}
	else if (_stricmp(szLang, "en-us") == 0)
	{
		strcpy(pData->szDisplayName, "English, United States (en-us)");
		result = TSC_S_OK;
	}
	else
	{
		result = TSC_S_FALSE;
	}
	
	return result;
}

tsc_result CSession::EnumLanguages(LanguageEnumFn pfn, void* pUserData)
{
	if (!IsInitialised())
		return Error_SessionNotInitialised();

	if (!pfn)
		return Error_ParamWasNull();
	
	tsc_result result = TSC_E_FAIL;
	
	LANGUAGE_DESC_WIDEDATA ldwd;
	
	memset(&ldwd, 0, sizeof(LANGUAGE_DESC_WIDEDATA));
	
	wcscpy(ldwd.wszDisplayName, L"English, United Kingdom");
	pfn(&ldwd, pUserData);
	
	wcscpy(ldwd.wszDisplayName, L"English, United States");
	pfn(&ldwd, pUserData);
	
	return Success();
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
