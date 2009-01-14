#ifndef _TOLON_SPELL_CHECK_H__
#define _TOLON_SPELL_CHECK_H__

#include <windef.h>

#ifdef __cplusplus
#define TSC_API extern "C"
#else
#define TSC_API extern
#endif

#define tsc_false 0

typedef tsc_bool unsigned char;
typedef tsc_size_t size_t;
typedef tsc_byte unsigned char;
typedef tsc_result unsigned long;
typedef tsc_cookie unsigned long;

#pragma pack(push)
#pragma pack(1)
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

typedef tsc_result (CALLBACK *CHECKSPELLING_CALLBACK)(tsc_session SessionID, tsc_byte* pBuffer, tsc_size_t BufferSize, tsc_size_t* pBytesInBuffer);
typedef tsc_result (CALLBACK *REPLACEWORD_CALLBACK)(tsc_session SessionID, tsc_size_t OriginalStart, tsc_size_t OriginalLength, tsc_byte* pReplacementBytes, tsc_size_t ReplacementLength);

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
#pragma pack(pop)

// Module-level functions

TSC_API
tsc_result tscGetVersion( TSC_VERSION_DATA* pData );

TSC_API
tsc_result tscCreateSession( tsc_cookie* pSessionID,
                             TSC_CREATESESSION_DATA* pData );

TSC_API
tsc_result tscDestroySession( tsc_cookie SessionID );

// Session-level functions
TSC_API
tsc_result tscsGetSessionOptions( tsc_cookie SessionID, 
                                  TSC_SESSIONOPTIONS_DATA* pData );

TSC_API
tsc_result tscsSetSessionOptions( tsc_cookie SessionID,
                                  TSC_SESSIONOPTIONS_DATA* pData );

TSC_API
tsc_result tscsShowOptionsWindow( tsc_cookie SessionID,
                                  TSC_SHOWOPTIONSWINDOW_DATA* pData );

TSC_API
tsc_result tscsCheckSpelling( tsc_cookie SessionID,
                              TSC_CHECKSPELLING_DATA* pData);

#endif
