#pragma once

#include "TolonSpellCheck.h"

namespace TolonSpellCheck {
	typedef struct tagLANGUAGE_DESC {
		tsc_size_t cbSize;
		char szDisplayName[128];
	} LANGUAGE_DESC_DATA;
	
	typedef struct tagLANGUAGE_DESC_WIDE {
		tsc_size_t cbSize;
		wchar_t wszDisplayName[128];
	} LANGUAGE_DESC_WIDEDATA;
	
	class CModule;
	class CSession;
	
	typedef bool (*LanguageEnumFn) (LANGUAGE_DESC_WIDEDATA* pData,
									  void * user_data);
};
