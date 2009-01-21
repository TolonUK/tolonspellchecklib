#ifndef _TOLON_SPELL_CHECK_H__
#define _TOLON_SPELL_CHECK_H__

#include <windef.h>
#include <winerror.h>

#ifdef __cplusplus
#define TSC_API extern "C"
#else
#define TSC_API extern
#endif

#define TSC_CALLTYPE __stdcall

#define tsc_false 0

typedef unsigned char tsc_bool;
typedef size_t tsc_size_t;
typedef unsigned char tsc_byte;
typedef unsigned long tsc_result;
typedef unsigned long tsc_cookie;

// Result Codes
#define TSC_S_FALSE (S_FALSE)
#define TSC_S_OK (S_OK)
#define TSC_E_FAIL (E_FAIL)
// TSC_E_INVALIDARG implies that one or more of the arguments
// passed to a function are invalid (e.g. NULL).
#define TSC_E_INVALIDARG (E_INVALIDARG)
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
typedef struct tagTSC_INIT {
	char	szAppName[32]; //short name used internally only
} TSC_INIT_DATA;

typedef struct tagTSC_VERSION {
    tsc_size_t cbSize;
    tsc_byte nMajor;
    tsc_byte nMinor;
    tsc_byte nReserved1;
    tsc_byte nReserver2;
} TSC_VERSION_DATA;

typedef struct tagTSC_CREATESESSION {
} TSC_CREATESESSION_DATA;

typedef struct tagTSC_SESSIONOPTIONS {
    tsc_bool bIgnoreUserDictionaries;
    tsc_bool bIgnoreUppercaseWords;
    tsc_bool bIgnoreWordsWithNumbers;
    tsc_bool bIgnoreUris;
    char     szDictionaryCulture[12]; //UTF8, allow for "ab-cd-efgh"
    char     szPreferredProvider[8]; // UTF8, { "aspell" | "ispell" | "myspell" | "uspell" | "voikko" | "zemb" }
    tsc_byte nReserved1;
    tsc_byte nReserved2;
} TSC_SESSIONOPTIONS_DATA;

typedef struct tagTSC_SHOWOPTIONSWINDOW {
    HWND hWndParent;
} TSC_SHOWOPTIONSWINDOW_DATA;

typedef tsc_result (CALLBACK *CHECKSPELLING_CALLBACK)(tsc_cookie SessionID, tsc_byte* pBuffer, tsc_size_t BufferSize, tsc_size_t* pBytesInBuffer);
typedef tsc_result (CALLBACK *REPLACEWORD_CALLBACK)(tsc_cookie SessionID, tsc_size_t OriginalStart, tsc_size_t OriginalLength, tsc_byte* pReplacementBytes, tsc_size_t ReplacementLength);

enum enumCheckSpellingTarget {
    TARGET_CALLBACKS,
    TARGET_EDIT,
    TARGET_RICHEDIT,
    TARGET_TOLON_TEXTLIB,
    
    TARGET_COUNT };

typedef struct tagTSC_CHECKSPELLING {
    HWND hWndParent;
    tsc_byte nTarget;
    tsc_byte nReserved1;
    tsc_byte nReserved2;
    tsc_byte nReserved3;
    CHECKSPELLING_CALLBACK pfnCallback;
} TSC_CHECKSPELLING_DATA;

typedef struct tagTSC_CHECKWORD {
	// In
	union {
		const char* szWord8; // UTF-8
		const wchar_t* szWord16; // Unicode
	} uTestWord;
	tsc_size_t nWordSize; // in bytes
	// In/Out
	union {
		char* szResults8; // UTF-8
		wchar_t szResults16; // Unicode
	} uResultString;
	tsc_size_t nResultStringSize; // IN: no. of bytes in result string, OUT: no. of bytes used
	// Out
	bool bOk;
} TSC_CHECKWORD_DATA;
#pragma pack(pop)

// Module-level functions
TSC_API tsc_result TSC_CALLTYPE 
	tscInit( TSC_INIT_DATA* pData );

TSC_API tsc_result TSC_CALLTYPE 
	tscUninit( void );

TSC_API tsc_result TSC_CALLTYPE 
	tscGetVersion( TSC_VERSION_DATA* pData );

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

#endif
