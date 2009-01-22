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
		m_pEnchantBroker = enchant_broker_init();

		if (m_pEnchantBroker)
		{
			m_pEnchantDict = enchant_broker_request_dict(m_pEnchantBroker, "en_US");

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
    
    return TSC_S_OK;
}
