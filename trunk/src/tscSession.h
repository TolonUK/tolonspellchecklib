#pragma once

#include "TolonSpellCheck.h"
typedef int ssize_t;
#include "enchant.h"

namespace TolonSpellCheck {
    
class CSession
{
public:
    CSession(TSC_CREATESESSION_DATA* pData);
    ~CSession();

	tsc_result Init();
	tsc_result Uninit();
	tsc_result CheckWord(const char* szWord);

    tsc_result ShowOptionsWindow();

	bool IsInitialised() const
		{ return m_bInitialised; }

	void SetInitialised(bool b)
		{ m_bInitialised = b; }

protected:
	bool m_bInitialised;
	EnchantBroker* m_pEnchantBroker;
	EnchantDict* m_pEnchantDict;
};

} //namespace