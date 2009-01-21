#include "tscModule.h"

using namespace TolonSpellCheck;
using namespace std;

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
	m_bInitialised(false)
{
}

CModule::~CModule()
{
}

tsc_result CModule::Init(const char * szHostName)
{
	tsc_result result = TSC_E_FAIL;

	if (!IsInitialised())
	{
		m_sHostName = szHostName;
		SetInitialised(true);
		result = TSC_S_OK;
	}

	return result;
}

tsc_result CModule::Uninit()
{
	tsc_result result = TSC_E_FAIL;

	if (IsInitialised())
	{
		m_sHostName.clear();
		SetInitialised(false);
		result = TSC_S_OK;
	}

	return result;
}