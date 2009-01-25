#pragma once

#include "TolonSpellCheck.h"

namespace TolonSpellCheck {
	typedef struct tagLANGUAGE_DESC {
		char szDisplayName[128];
	} LANGUAGE_DESC_DATA;
	
	typedef struct tagLANGUAGE_DESC_WIDE {
		wchar_t wszDisplayName[128];
	} LANGUAGE_DESC_WIDEDATA;
	
	class CModule;
	class CSession;
};
