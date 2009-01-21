#pragma once

#include "TolonSpellCheck.h"
#include <string>

namespace TolonSpellCheck {
    
class CModule
{
public:
    static CModule* GetInstance();
    
protected:
    CModule();
    ~CModule();

public:
	tsc_result Init(const char * szHostName);
	tsc_result Uninit();

	bool IsInitialised() const
		{ return m_bInitialised; }

protected:
	void SetInitialised(bool b)
		{ m_bInitialised = b; }

// Member variables
protected:
	static CModule* sm_pThis;

	std::string m_sHostName;
	bool m_bInitialised;
};

}; //namespace