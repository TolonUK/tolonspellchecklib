// Test info for DLL functions
//
//               tscInit: test_abnormal_init_uninit, test_normal_init_uninit
//             tscUninit: test_abnormal_init_uninit, test_no_init, test_normal_init_uninit
//         tscGetVersion: test_getversion
//       tscGetLastError: 
//      tscCreateSession: test_createsession
//     tscDestroySession: test_destroysession
//  tscGetSessionOptions  |
//  tscSetSessionOptions  |
//  tscShowOptionsWindow  |
//      tscCheckSpelling  |
//          tscCheckWord  |

#define _CRT_SECURE_NO_WARNINGS 1

#include "TolonSpellCheck.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <windows.h>
#include <commctrl.h>

#pragma comment(linker, \
    "\"/manifestdependency:type='Win32' "\
    "name='Microsoft.Windows.Common-Controls' "\
    "version='6.0.0.0' "\
    "processorArchitecture='*' "\
    "publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

using namespace std;

void util_begin_test(char* szPreamble);
void util_end_test(bool bResult);
void util_is_expected(tsc_result r, bool& bSetFalseIfFailed);
void util_is_failure(const char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfNotFailed);
void util_is_success(const char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfFailed);
const char* util_textify_result(tsc_result r);
void check_test(bool bResult);
void print_stats();
// The tests
bool test_abnormal_init_uninit();
bool test_createsession();
bool test_destroysession();
bool test_getsessionoptions();
bool test_getversion();
bool test_no_init();
bool test_normal_init_uninit();
bool test_null_params();
bool test_show_options();
bool test_word();
bool test_user_dic();
// Subtests
void subtest_tscInit(bool& bTestResult);
void subtest_tscCreateSession(tsc_cookie& c, bool& bTestResult);
void subtest_tscDestroySession(tsc_cookie c, bool& bTestResult);
void subtest_tscUninit(bool& bTestResult);

static string s_sTextResult;
static int g_nTestStartCount, g_nTestEndCount, g_nSuccessCount, g_nFailCount;
static const size_t g_nTimingLimit = 10;
static std::stack<DWORD> g_Timings;
static const char* g_szLine = "--------------------------------------------------------------------------------";

int main()
{	
    g_nTestStartCount = g_nTestEndCount = g_nSuccessCount = g_nFailCount = 0;

    ::InitCommonControls();
    
    cout << "Tolon Spell Check Test Program. Copyright (c) 2009-2011 Alex Paterson." << endl;
    cout << g_szLine << endl;
    
    // Init/Uninit
    check_test(test_abnormal_init_uninit());
    check_test(test_normal_init_uninit());
    check_test(test_no_init());

    // Create/Destroy Session
    check_test(test_createsession());
    check_test(test_destroysession());

    // Misc
    check_test(test_getsessionoptions());
    check_test(test_getversion());
    check_test(test_null_params());

    // Spell Checking
    check_test(test_word());
	check_test(test_user_dic());
    check_test(test_show_options());
    
    print_stats();
    
    return 0;
}

// Utils

void check_test(bool bResult)
{
    if (bResult)
        ++g_nSuccessCount;
    else
        ++g_nFailCount;
}

void print_stats()
{
    cout << g_nTestStartCount << " tests started, " << g_nTestEndCount << " tests finished." << endl;
    cout << g_nSuccessCount << " ok, " << g_nFailCount << " failed." << endl;
    cout << g_szLine << endl;
    cout << endl;
}

void util_begin_test(char* szPreamble)
{
    g_Timings.push(GetTickCount());

    ++g_nTestStartCount;
    cout << szPreamble << endl;
}

void util_end_test(bool bResult)
{
    DWORD dwTicks = 0;
    if (!g_Timings.empty())
    {
        dwTicks = GetTickCount() - g_Timings.top();
        g_Timings.pop();
    }

    ++g_nTestEndCount;
    if (bResult)
        cout << "Test ok";
    else
        cout << "TEST FAILED";
    cout << ", took " << dwTicks << "ms" << endl << g_szLine << endl;
}

template<typename T>
void util_is_expected(char* szTestPartName, const T& expected, const T& actual, bool& bSetFalseIfFailed)
{
	if (actual == expected) {
		cout << "\t" << szTestPartName << " ok" << endl;
	}
	else {
		cout << "FAIL->\t" << szTestPartName << " FAIL; expected " << expected << ", got " << actual << endl;
		bSetFalseIfFailed = false;
	}
}

template<>
void util_is_expected(char* szTestPartName, const tsc_result& rExpected, const tsc_result& rActual, bool& bSetFalseIfFailed)
{
    if (rActual == rExpected) {
        cout << "\t" << szTestPartName << " ok" << endl;
    }
    else {
        cout << "FAIL->\t" << szTestPartName << " FAIL; expected " << util_textify_result(rExpected) << ", got " << util_textify_result(rActual) << endl;
        bSetFalseIfFailed = false; 
    }
    cout << "\t\tLastError: " <<  ::tscGetLastError() << endl;
}

void util_is_failure(const char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfNotFailed)
{
    if (FAILED(rActual)) {
        cout << "\t" << szTestPartName <<" ok" << endl;
    }
    else {
        cout << "FAIL->\t" << szTestPartName <<" FAIL; expected failure, got " << util_textify_result(rActual) << endl;
        bSetFalseIfNotFailed = false; 
    }
    cout << "\t\tLastError: " <<  ::tscGetLastError() << endl;
}

void util_is_success(const char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfFailed)
{
    if (SUCCEEDED(rActual)) {
        cout << "\t" << szTestPartName <<" ok" << endl;
    }
    else {
        cout << "FAIL->\t" << szTestPartName <<" FAIL; expected success, got " << util_textify_result(rActual) << endl;
        bSetFalseIfFailed = false; 
    }
    cout << "\t\tLastError: " <<  ::tscGetLastError() << endl;
}

const char* util_textify_result(tsc_result r)
{
    switch(r)
    {
        case TSC_S_FALSE:
            return "TSC_S_FALSE";
        case TSC_S_OK:
            return "TSC_S_OK";
        case TSC_E_FAIL:
            return "TSC_E_FAIL";
        case TSC_E_INVALIDARG:
            return "TSC_E_INVALIDARG";
        case TSC_E_NOTIMPL:
            return "TSC_E_NOTIMPL";
        case TSC_E_POINTER:
            return "TSC_E_POINTER";
        case TSC_E_UNEXPECTED:
            return "TSC_E_UNEXPECTED";
        default:
            stringstream ss;
            ss << "unknown 0x" << setw(8) << hex << r;
			ss.str().swap(s_sTextResult);
            return s_sTextResult.c_str();
    }
}

// Tests below are in alphabetical order

bool test_abnormal_init_uninit()
{
    bool bTestResult = true;
    tsc_result r;
    TSC_INIT_DATA id;
    
    util_begin_test("test_abnormal_init_uninit()\r\nTesting abnormal tscInit/tscUninit sequences...");
    
    memset(&id, 0xff, sizeof(TSC_INIT_DATA));
    id.cbSize = sizeof(TSC_INIT_DATA);
	id.nOptions = 0;
    strcpy(id.szAppName, "TestAppName01234567890123456789");
    
    // Test uninit without init
    r = ::tscUninit();
    util_is_failure("tscUninit without tscInit", r, bTestResult);
    
    // Test double init
    r = ::tscInit(&id);
    util_is_success("double tscInit, first call", r, bTestResult);
    r = ::tscInit(&id);
    util_is_failure("double tscInit, second call", r, bTestResult);
    
    // Test double uninit
    r = ::tscUninit();
    util_is_success("double tscUninit, first call", r, bTestResult);
    r = ::tscUninit();
    util_is_failure("double tscUninit, second call", r, bTestResult);
    
    util_end_test(bTestResult);
    
    return bTestResult;
}

bool test_createsession()
{
    bool bTestResult = true;
    tsc_result r;

    util_begin_test("test_createsession()\r\nTesting normal and abnormal tscCreateSession calls...");

    // Prologue
    subtest_tscInit(bTestResult);

    tsc_cookie c = TSC_NULL_COOKIE;
    TSC_CREATESESSION_DATA cs;
    memset(&cs, 0x00, sizeof(TSC_CREATESESSION_DATA));

    // Test null parameters
    {
        r = ::tscCreateSession(NULL, &cs);
        util_is_failure("tscCreateSession, null cookie ptr", r, bTestResult);

        r = ::tscCreateSession(&c, NULL);
        util_is_failure("tscCreateSession, null data ptr", r, bTestResult);
    }

    // Test uninitialized cbSize
    {
        r = ::tscCreateSession(&c, &cs);
        util_is_failure("tscCreateSession, null uninitialized cbSize", r, bTestResult);
    }

    // Test normal
    {
        cs.cbSize = sizeof(TSC_CREATESESSION_DATA);

        r = ::tscCreateSession(&c, &cs);
        util_is_success("tscCreateSession", r, bTestResult);
    }

    // Epilogue
    subtest_tscDestroySession(c, bTestResult);
    subtest_tscUninit(bTestResult);

    util_end_test(bTestResult);

    return bTestResult;
}

bool test_destroysession()
{
    bool bTestResult = true;
    tsc_result r;

    util_begin_test("test_destroysession()\r\nTesting normal and abnormal tscDestroySession calls...");

    // Prologue
    subtest_tscInit(bTestResult);

    // Test invalid parameters
    {
        r = ::tscDestroySession(0);
        util_is_failure("tscDestroySession, invalid zero cookie", r, bTestResult);

        r = ::tscDestroySession(1);
        util_is_failure("tscDestroySession, invalid non-zero cookie", r, bTestResult);
    }

    // Test normal + double destroy
    {
        tsc_cookie c = TSC_NULL_COOKIE;
        TSC_CREATESESSION_DATA cs;
        memset(&cs, 0x00, sizeof(TSC_CREATESESSION_DATA));
        cs.cbSize = sizeof(TSC_CREATESESSION_DATA);

        r = ::tscCreateSession(&c, &cs);
        util_is_success("tscCreateSession", r, bTestResult);

        r = ::tscDestroySession(c);
        util_is_success("tscDestroySession, normal", r, bTestResult);

        r = ::tscDestroySession(c);
        util_is_failure("tscDestroySession, second-destroy", r, bTestResult);
    }

    // Epilogue
    subtest_tscUninit(bTestResult);

    util_end_test(bTestResult);

    return bTestResult;
}

bool test_getsessionoptions()
{
    bool bTestResult = true;
    tsc_result r;
    tsc_cookie c = TSC_NULL_COOKIE;
    
    util_begin_test("test_getsessionoptions()\r\nTesting tscGetSessionOptions...");
    
    // Prologue
    subtest_tscInit(bTestResult);
    subtest_tscCreateSession(c, bTestResult);
    
    // tscGetSessionOptions
    TSC_SESSIONOPTIONS_DATA sod;
    memset(&sod, 0xff, sizeof(TSC_SESSIONOPTIONS_DATA));

    // null data ptr
    r = ::tscGetSessionOptions(c, NULL);
    util_is_failure("tscGetSessionOptions, null data ptr", r, bTestResult);

    // invalid size structure
    sod.cbSize = 0;
    r = ::tscGetSessionOptions(c, &sod);
    util_is_failure("tscGetSessionOptions, zero cbSize", r, bTestResult);

    sod.cbSize = sizeof(TSC_SESSIONOPTIONS_DATA);
    r = ::tscGetSessionOptions(c, &sod);
    util_is_success("tscGetSessionOptions, normal", r, bTestResult);    

    // Epilogue
    subtest_tscDestroySession(c, bTestResult);
    subtest_tscUninit(bTestResult);
    
    util_end_test(bTestResult);
    
    return bTestResult;
}

bool test_getversion()
{
    bool bTestResult = true;
    tsc_result r;
    TSC_INIT_DATA id;
    TSC_VERSION_DATA vd;
    
    util_begin_test("test_getversion()\r\nTesting tscGetVersion...");
    
    memset(&id, 0xff, sizeof(TSC_INIT_DATA));
    id.cbSize = sizeof(TSC_INIT_DATA);
	id.nOptions = 0;
    strcpy(id.szAppName, "TestAppName01234567890123456789");
    r = ::tscInit(&id);
    
    memset(&vd, 0xff, sizeof(TSC_VERSION_DATA));
    vd.cbSize = sizeof(TSC_VERSION_DATA);
    r = ::tscGetVersion(&vd);
    util_is_success("tscGetVersion", r, bTestResult);
    
    r = ::tscUninit();
    
    util_end_test(bTestResult);
    
    return bTestResult;
}

bool test_no_init()
{
    bool bTestResult = true;
    const tsc_result rExpected = E_UNEXPECTED;
    tsc_result r = 0;
    
    util_begin_test("test_no_init()\r\nTesting API use without calling tscInit() first...");
    
    // Because the library has not been initialised, the functions below
    // should return TSC_E_UNEXPECTED without attempting to use any of the
    // parameters. Hence passing NULL should be OK.
    r = ::tscUninit();
    util_is_expected("tscUninit", rExpected, r, bTestResult);
    
    {
        TSC_VERSION_DATA vd = {0};
        vd.cbSize = sizeof(TSC_VERSION_DATA);
        r = ::tscGetVersion(&vd);
        util_is_expected("tscGetVersion", rExpected, r, bTestResult);
    }
    
    {
        tsc_cookie tc = 0;
        TSC_CREATESESSION_DATA csd = {0};
        csd.cbSize = sizeof(TSC_CREATESESSION_DATA);
        r = ::tscCreateSession(&tc, &csd);
        util_is_expected("tscCreateSession", rExpected, r, bTestResult);
    }
    
    r = ::tscDestroySession(TSC_NULL_COOKIE);
    util_is_expected("tscDestroySession", rExpected, r, bTestResult);
    
    {
        TSC_SESSIONOPTIONS_DATA sod = {0};
        sod.cbSize = sizeof(TSC_SESSIONOPTIONS_DATA);
        r = ::tscGetSessionOptions(TSC_NULL_COOKIE, &sod);
        util_is_expected("tscGetSessionOptions", rExpected, r, bTestResult);
    }
    
    {
        TSC_SESSIONOPTIONS_DATA sod = {0};
        sod.cbSize = sizeof(TSC_SESSIONOPTIONS_DATA);
        r = ::tscSetSessionOptions(TSC_NULL_COOKIE, &sod);
        util_is_expected("tscSetSessionOptions", rExpected, r, bTestResult);
    }
    
    {
        TSC_SHOWOPTIONSWINDOW_DATA sowd = {0};
        sowd.cbSize = sizeof(TSC_SHOWOPTIONSWINDOW_DATA);
        r = ::tscShowOptionsWindow(TSC_NULL_COOKIE, &sowd);
        util_is_expected("tscShowOptionsWindow", rExpected, r, bTestResult);
    }

    {
        TSC_CHECKSPELLING_DATA csd = {0};
        csd.cbSize = sizeof(TSC_CHECKSPELLING_DATA);
        r = ::tscCheckSpelling(TSC_NULL_COOKIE, &csd);
        util_is_expected("tscCheckSpelling", rExpected, r, bTestResult);
    }

    {
        TSC_CHECKWORD_DATA cwd = {0};
        cwd.cbSize = sizeof(TSC_CHECKWORD_DATA);
        r = ::tscCheckWord(TSC_NULL_COOKIE, &cwd);
        util_is_expected("tscCheckWord", rExpected, r, bTestResult);
    }

    util_end_test(bTestResult);
    
    return bTestResult;
}

bool test_normal_init_uninit()
{
    bool bTestResult = true;
    tsc_result r;
    TSC_INIT_DATA id;
    
    util_begin_test("test_normal_init_uninit()\r\nTesting normal tscInit/tscUninit sequence...");
    
    memset(&id, 0xff, sizeof(TSC_INIT_DATA));
    id.cbSize = sizeof(TSC_INIT_DATA);
	id.nOptions = 0;
    strcpy(id.szAppName, "TestAppName01234567890123456789");
    
    r = ::tscInit(&id);
    util_is_success("tscInit", r, bTestResult);
    
    r = ::tscUninit();
    util_is_success("tscUninit", r, bTestResult);
    
    util_end_test(bTestResult);
    
    return bTestResult;
}

bool test_null_params()
{
    bool bTestResult = true;
    const tsc_result rExpected = TSC_E_FAIL;
    tsc_result r = 0;
    
    util_begin_test("test_null_params()\r\nTesting API use with null parameters...");
    
    // Because the parameters are null, a dll error should be returned.
    r = ::tscGetVersion(NULL);
    util_is_expected("tscGetVersion", rExpected, r, bTestResult);
    
    r = ::tscCreateSession(NULL, NULL);
    util_is_expected("tscCreateSession", rExpected, r, bTestResult);
    
    r = ::tscGetSessionOptions(TSC_NULL_COOKIE, NULL);
    util_is_expected("tscGetSessionOptions", rExpected, r, bTestResult);
    
    r = ::tscSetSessionOptions(TSC_NULL_COOKIE, NULL);
    util_is_expected("tscSetSessionOptions", rExpected, r, bTestResult);
    
    r = ::tscShowOptionsWindow(TSC_NULL_COOKIE, NULL);
    util_is_expected("tscShowOptionsWindow", rExpected, r, bTestResult);
    
    r = ::tscCheckSpelling(TSC_NULL_COOKIE, NULL);
    util_is_expected("tscCheckSpelling", rExpected, r, bTestResult);
    
    r = ::tscCheckWord(TSC_NULL_COOKIE, NULL);
    util_is_expected("tscCheckWord", rExpected, r, bTestResult);
    
    util_end_test(bTestResult);
    
    return bTestResult;
}

bool test_show_options()
{
    bool bTestResult = true;
    tsc_result r;
    tsc_cookie c = TSC_NULL_COOKIE;
    
    util_begin_test("test_show_options()\r\nTesting tscShowOptionsWindow...");
    
    // Init module
    TSC_INIT_DATA id;
    memset(&id, 0xff, sizeof(TSC_INIT_DATA));
    id.cbSize = sizeof(TSC_INIT_DATA);
	id.nOptions = 0;
    strcpy(id.szAppName, "TestAppName01234567890123456789");
    r = ::tscInit(&id);
    util_is_success("tscInit", r, bTestResult); 
    
    // Create session
    TSC_CREATESESSION_DATA cs;
    memset(&cs, 0xff, sizeof(TSC_CREATESESSION_DATA));
    cs.cbSize = sizeof(TSC_CREATESESSION_DATA);
    r = ::tscCreateSession(&c, &cs);
    util_is_success("tscCreateSession", r, bTestResult); 
    
    // Show options window
    TSC_SHOWOPTIONSWINDOW_DATA sow;
    memset(&sow, 0xff, sizeof(TSC_SHOWOPTIONSWINDOW_DATA));
    sow.cbSize = sizeof(TSC_SHOWOPTIONSWINDOW_DATA);
    sow.hwndParent = NULL;
    r = ::tscShowOptionsWindow(c, &sow);   
    util_is_success("tscShowOptionsWindow", r, bTestResult);
    // and again to show that the options were stored in memory
    r = ::tscShowOptionsWindow(c, &sow);   
    util_is_success("tscShowOptionsWindow", r, bTestResult);
    
    // Destroy session
    r = ::tscDestroySession(c);
    util_is_success("tscDestroySession", r, bTestResult);  
    
    // Uninit module
    r = ::tscUninit();
    util_is_success("tscUninit", r, bTestResult);  
    
    util_end_test(bTestResult);
    
    return bTestResult;
}

bool DoCheckWord(tsc_cookie c, const char* szWord, bool& bWordOk, bool& bTestResult)
{
	tsc_result r = TSC_E_UNEXPECTED;

	TSC_CHECKWORD_DATA cw;
	memset(&cw, 0xff, sizeof(TSC_CHECKWORD_DATA));
	cw.cbSize = sizeof(TSC_CHECKWORD_DATA);
	cw.sTestWord = szWord;
	cw.sResults = NULL;
	cw.nResultStringSize = 0;
	r = ::tscCheckWord(c, &cw);

	{
		std::stringstream ss;
		ss << "tscCheckWord(\"" << szWord << "\"), fn call";
		util_is_success(ss.str().c_str(), r, bTestResult);
	}

	bool bCallOk = TSC_SUCCEEDED(r);
	if (bCallOk)
		bWordOk = cw.bOk;
	return bCallOk;
}

bool test_word()
{
    bool bTestResult = true;
	bool bWordOk = false;
    tsc_cookie c = TSC_NULL_COOKIE;
	const char* szGoodWordToTest = "hello";
    const char* szBadWordToTest = "helllllo";
    
    util_begin_test("test_word()\r\nTesting tscCheckWord(\"hello\") and tscCheckWord(\"helllllo\")...");
    
    // Prologue
    subtest_tscInit(bTestResult);
    subtest_tscCreateSession(c, bTestResult);
    
    // CheckWord Good
	if (DoCheckWord(c, szGoodWordToTest, bWordOk, bTestResult))
	{
		util_is_expected("tscCheckWord(\"hello\"), check result", true, bWordOk, bTestResult);
	}

	// Checkword Bad
	if (DoCheckWord(c, szBadWordToTest, bWordOk, bTestResult))
	{
		util_is_expected("tscCheckWord(\"hellllo\"), check result", false, bWordOk, bTestResult);
	}

    // Epilogue
    subtest_tscDestroySession(c, bTestResult);
    subtest_tscUninit(bTestResult);
    
    util_end_test(bTestResult);
    
    return bTestResult;
}

bool test_user_dic()
{
    bool bTestResult = true;
    tsc_result r;
	bool bWordOk = false;
    tsc_cookie c = TSC_NULL_COOKIE;
    const char* szWordToTest = "discombobulated";
    
    util_begin_test("test_user_dic()\r\nTesting tscCheckWord(\"discombobulated\")...");
    
    // Prologue
    subtest_tscInit(bTestResult);
    subtest_tscCreateSession(c, bTestResult);
    
    // CheckWord not present
	if (DoCheckWord(c, szWordToTest, bWordOk, bTestResult))
	{
		util_is_expected("tscCheckWord(\"discombobulated\"), check result", false, bWordOk, bTestResult);
	}

	// add user dic word here
	{
		TSC_CUSTOMDIC_DATA cdd;
		memset(&cdd, 0xff, sizeof(TSC_CUSTOMDIC_DATA));
		cdd.cbSize = sizeof(TSC_CUSTOMDIC_DATA);
		cdd.sWord = szWordToTest;
		cdd.nCustomDicAction = CUSTOMDICACTION_ADDWORD;
		r = ::tscCustomDic(c, &cdd);
		util_is_success("tscCustomDic(add discombobulated), result", r, bTestResult);
	}

	// Checkword present
	if (DoCheckWord(c, szWordToTest, bWordOk, bTestResult))
	{
		util_is_expected("tscCheckWord(\"discombobulated\"), check result", true, bWordOk, bTestResult);
	}

	//remove user dic word here
	{
		TSC_CUSTOMDIC_DATA cdd;
		memset(&cdd, 0xff, sizeof(TSC_CUSTOMDIC_DATA));
		cdd.cbSize = sizeof(TSC_CUSTOMDIC_DATA);
		cdd.sWord = szWordToTest;
		cdd.nCustomDicAction = CUSTOMDICACTION_REMOVEWORD;
		r = ::tscCustomDic(c, &cdd);
		util_is_success("tscCustomDic(remove discombobulated), result", r, bTestResult);
	}

	// Checkword not present
	if (DoCheckWord(c, szWordToTest, bWordOk, bTestResult))
	{
		util_is_expected("tscCheckWord(\"discombobulated\"), check result", false, bWordOk, bTestResult);
	}

    // Epilogue
    subtest_tscDestroySession(c, bTestResult);
    subtest_tscUninit(bTestResult);
    
    util_end_test(bTestResult);
    
    return bTestResult;
}

void subtest_tscInit(bool& bTestResult)
{
    tsc_result r = TSC_E_FAIL;
    TSC_INIT_DATA id;
    memset(&id, 0xff, sizeof(TSC_INIT_DATA));
    id.cbSize = sizeof(TSC_INIT_DATA);
	id.nOptions = 0;
    strcpy(id.szAppName, "TestAppName01234567890123456789");
    r = ::tscInit(&id);
    util_is_success("tscInit", r, bTestResult); 
}

void subtest_tscCreateSession(tsc_cookie& c, bool& bTestResult)
{
    tsc_result r = TSC_E_FAIL;
    TSC_CREATESESSION_DATA cs;
    memset(&cs, 0xff, sizeof(TSC_CREATESESSION_DATA));
    cs.cbSize = sizeof(TSC_CREATESESSION_DATA);
    r = ::tscCreateSession(&c, &cs);
    util_is_success("tscCreateSession", r, bTestResult); 
}

void subtest_tscDestroySession(tsc_cookie c, bool& bTestResult)
{
    tsc_result r = TSC_E_FAIL;
    r = ::tscDestroySession(c);
    util_is_success("tscDestroySession", r, bTestResult);  
}

void subtest_tscUninit(bool& bTestResult)
{
    tsc_result r = TSC_E_FAIL;
    r = ::tscUninit();
    util_is_success("tscUninit", r, bTestResult);  
}
