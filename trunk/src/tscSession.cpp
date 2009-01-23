#include "tscSession.h"
#include "enchant.h"
#include "SpellingOptionsDlg.h"
#include <afxwin.h>
#include <afxcmn.h>

using namespace TolonSpellCheck;

CSession::CSession(TSC_CREATESESSION_DATA* pData) :
	m_bInitialised(false),
	m_pEnchantBroker(NULL)
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
	tsc_result result = TSC_E_FAIL;

	if (IsInitialised())
	{
		result = TSC_E_UNEXPECTED;
		//TODO: error msg here
	}
	else
	{
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
		
		MessageBoxA(NULL, m_options.szDictionaryCulture, NULL, MB_OK);
		
		// Initialise the enchant library
		m_pEnchantBroker = enchant_broker_init();

		if (m_pEnchantBroker)
		{
			m_pEnchantDict = enchant_broker_request_dict(m_pEnchantBroker, m_options.szDictionaryCulture);

			if (m_pEnchantDict)
			{
				result = TSC_S_OK;
				SetInitialised(true);
			}
			else
			{
				enchant_broker_free(m_pEnchantBroker);
				m_pEnchantBroker = NULL;
			}
		}
	}

	return result;
}

tsc_result CSession::Uninit()
{
	tsc_result result = TSC_E_FAIL;

	if (!IsInitialised())
	{
		result = TSC_E_UNEXPECTED;
		//TODO: err msg here
	}
	else
	{
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
	}

	return result;
}

tsc_result CSession::CheckWord(const char* szWord)
{
	tsc_result result = TSC_E_FAIL;

	if (!IsInitialised())
		return TSC_E_UNEXPECTED;

	if (!szWord)
		return TSC_E_INVALIDARG;

	if ( enchant_dict_check(m_pEnchantDict, szWord, -1) == 0 )
	{
		// word found
		result = TSC_S_OK;
	}
	else
	{
		// can't find word, get suggestions
		result = TSC_S_FALSE;

		char **suggs = NULL;
		size_t n_suggs = 0;

		suggs = enchant_dict_suggest( m_pEnchantDict,
									  szWord,
									  strlen(szWord),
									  &n_suggs );

		if (suggs && n_suggs)
			enchant_dict_free_string_list(m_pEnchantDict, suggs);
	}

	return result;
}

tsc_result CSession::ShowOptionsWindow()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (!IsInitialised())
		return TSC_E_UNEXPECTED;
	
	CSpellingOptionsDlg dlg;
	
	dlg.DoModal();
	
	enchant_broker_list_dicts (m_pEnchantBroker, CSession::cbEnchantDictDescribe, NULL);
	
	return TSC_S_OK;
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
	
	CString s;
	
	s.Format(_T("%s, %s, %s"), wszLangTag, wszProvName, wszProvDesc);
	
	AfxMessageBox(s);
}
