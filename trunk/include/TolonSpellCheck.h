#ifndef _TOLON_SPELL_CHECK_H__
#define _TOLON_SPELL_CHECK_H__

#pragma warning(push)
#pragma warning(disable:4201) // warning C4201: nonstandard extension used : nameless struct/union
#include <windows.h>
#pragma warning(pop)

#ifdef __cplusplus
#define TSC_API extern "C"
#else
#define TSC_API extern
#endif

#define TSC_VERSION_MAJOR	0
#define TSC_VERSION_MINOR	0

#define TSC_CALLTYPE __stdcall

#define tsc_false 0

typedef unsigned char tsc_bool;
typedef size_t tsc_size_t;
typedef unsigned char tsc_byte;
typedef long tsc_result;
typedef unsigned long tsc_cookie;

// Result Codes
#define TSC_S_FALSE (S_FALSE)
#define TSC_S_OK (S_OK)
#define TSC_E_FAIL (E_FAIL)
// TSC_E_INVALIDARG implies that one or more of the arguments
// passed to a function are invalid (e.g. struct size incorrect).
#define TSC_E_INVALIDARG (E_INVALIDARG)
// TSC_E_NOTIMPL indicates the function is not fully implemented.
#define TSC_E_NOTIMPL (E_NOTIMPL)
// TSC_E_POINTER implies that one or more pointer arguments
// were null.
#define TSC_E_POINTER (E_POINTER)
// TSC_E_UNEXPECTED implies that the call was not expected, 
// such as before the library has been initialised.
#define TSC_E_UNEXPECTED (E_UNEXPECTED)

// Result code helpers
#define TSC_SUCCEEDED(x) SUCCEEDED(x)
#define TSC_FAILED(x) FAILED(x)

// Cookie values
#define TSC_NULL_COOKIE (0)

// Structures
#pragma pack(push)
#pragma pack(1)
enum enumOptions {
	OPTION_PORTABLE_APP_MODE = 1,
	OPTION_2 = 2,
	OPTION_3 = 4,
	OPTION_4 = 8 };

typedef struct tagTSC_INIT {
	tsc_size_t cbSize;
	tsc_size_t nOptions;
	char szAppName[32]; //short name used internally only
} TSC_INIT_DATA;

typedef struct tagTSC_VERSION {
	tsc_size_t cbSize;
	tsc_byte nMajor;
	tsc_byte nMinor;
	tsc_byte nReserved1;
	tsc_byte nReserved2;
} TSC_VERSION_DATA;

typedef struct tagTSC_CREATESESSION {
	tsc_size_t cbSize;
} TSC_CREATESESSION_DATA;

typedef struct tagTSC_SESSIONOPTIONS {
	tsc_size_t cbSize;
	tsc_bool bIgnoreUserDictionaries;
	tsc_bool bIgnoreUppercaseWords;
	tsc_bool bIgnoreWordsWithNumbers;
	tsc_bool bIgnoreUris;
	char     szDictionaryCulture[12]; //UTF8, allow for "abc-def-ghij" (TODO)
	char     szPreferredProvider[8]; // UTF8, { "aspell" | "ispell" | "myspell" | "uspell" | "voikko" | "zemb" }
	tsc_byte nReserved1;
	tsc_byte nReserved2;
} TSC_SESSIONOPTIONS_DATA;

typedef struct tagTSC_SHOWOPTIONSWINDOW {
	tsc_size_t cbSize;
	HWND hwndParent;
} TSC_SHOWOPTIONSWINDOW_DATA;

typedef tsc_result (CALLBACK *CHECKSPELLING_CALLBACK)(tsc_cookie SessionID, tsc_byte* pBuffer, tsc_size_t BufferSize, tsc_size_t* pBytesInBuffer);
typedef tsc_result (CALLBACK *REPLACEWORD_CALLBACK)(tsc_cookie SessionID, tsc_size_t OriginalStart, tsc_size_t OriginalLength, tsc_byte* pReplacementBytes, tsc_size_t ReplacementLength);

enum enumCheckSpellingTarget {
	TARGET_CALLBACKS,
	TARGET_EDIT,
	TARGET_RICHEDIT,
	TARGET_TOLON_TEXTLIB,
	
	TARGET_COUNT };

enum enumCheckSpellingMode {
	MODE_AUTO,		//!< Interfaces directly with text or richtext control to do spell checking on its current contents.
	MODE_BUFFER,	//!< Uses a callback to request buffer chunks to check.
	MODE_WORD,		//!< Used a callback to request the next word to check.

	MODE_COUNT };

typedef struct tagTSC_CHECKSPELLING {
	tsc_size_t cbSize;
	HWND hwnd;	//!< If nMode = MODE_AUTO, this is window handle for the control to check the text of.
    HWND hwndParent; //!< The parent window for the spell checking dialog.
	tsc_byte nTarget;
	tsc_byte nMode;
	tsc_byte nReserved2;
	tsc_byte nReserved3;
	CHECKSPELLING_CALLBACK pfnCallback;
} TSC_CHECKSPELLING_DATA;

typedef struct tagTSC_CHECKWORD {
	tsc_size_t cbSize;
	// In
	const char* sTestWord; // UTF-8
	tsc_size_t nWordSize; // in bytes
	// In/Out
	char* sResults; // UTF-8
	tsc_size_t nResultStringSize; // IN: no. of bytes in result string, OUT: no. of bytes used
	// Out
	bool bOk;
} TSC_CHECKWORD_DATA;

/*typedef struct tagTSC_CHECKWORD {
	tsc_size_t cbSize;
	// In
	union {
		const char* szWord8; // UTF-8
		const wchar_t* szWord16; // Unicode
	} uTestWord;
	tsc_size_t nWordSize; // in bytes
	// In/Out
	union {
		char* szResults8; // UTF-8
		wchar_t* szResults16; // Unicode
	} uResultString;
	tsc_size_t nResultStringSize; // IN: no. of bytes in result string, OUT: no. of bytes used
	// Out
	bool bOk;
} TSC_CHECKWORD_DATA;*/

enum enumCustomDicAction {
	CUSTOMDICACTION_ADDWORD,		//!< Add the specified word to the custom dictionary.
	CUSTOMDICACTION_REMOVEWORD,	//!< Remove the specified word from the custom dictionary.

	CUSTOMDICACTION_COUNT };

typedef struct tagTSC_CUSTOMDIC {
	tsc_size_t cbSize;
	// In
	const char* sWord; // UTF-8
	tsc_size_t nWordSize; // in bytes
	tsc_byte nCustomDicAction;
	// Out
	bool bOk;
} TSC_CUSTOMDIC_DATA;

#pragma pack(pop)

// Module-level functions
TSC_API tsc_result TSC_CALLTYPE 
	tscInit( TSC_INIT_DATA* pData );

TSC_API tsc_result TSC_CALLTYPE 
	tscUninit( void );

TSC_API tsc_result TSC_CALLTYPE 
	tscGetVersion( TSC_VERSION_DATA* pData );

TSC_API const char* TSC_CALLTYPE
	tscGetLastError( void );

TSC_API tsc_result TSC_CALLTYPE 
	tscCreateSession( tsc_cookie* pSessionID,
					  TSC_CREATESESSION_DATA* pData );

TSC_API tsc_result TSC_CALLTYPE 
	tscDestroySession( tsc_cookie SessionID );

// Session-level functions
TSC_API tsc_result TSC_CALLTYPE 
	tscGetSessionOptions( tsc_cookie SessionID, 
						   TSC_SESSIONOPTIONS_DATA* pData );

TSC_API tsc_result TSC_CALLTYPE 
	tscSetSessionOptions( tsc_cookie SessionID,
						   TSC_SESSIONOPTIONS_DATA* pData );

TSC_API tsc_result TSC_CALLTYPE 
	tscShowOptionsWindow( tsc_cookie SessionID,
						   TSC_SHOWOPTIONSWINDOW_DATA* pData );

TSC_API tsc_result TSC_CALLTYPE 
	tscCheckSpelling( tsc_cookie SessionID,
					   TSC_CHECKSPELLING_DATA* pData );

TSC_API tsc_result TSC_CALLTYPE
	tscCheckWord( tsc_cookie SessionID,
				  TSC_CHECKWORD_DATA* pData );

TSC_API tsc_result TSC_CALLTYPE
	tscCustomDic( tsc_cookie SessionID,
				  TSC_CUSTOMDIC_DATA* pData );

#endif
