#include "TolonSpellCheck.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void util_begin_test(char* szPreamble);
void util_end_test(bool bResult);
void util_is_expected(tsc_result r, bool& bSetFalseIfFailed);
void util_is_failure(char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfNotFailed);
void util_is_success(char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfFailed);
const char* util_textify_result(tsc_result r);
void check_test(bool bResult);
void print_stats();
// The tests
bool test_abnormal_init_uninit();
bool test_getversion();
bool test_no_init();
bool test_normal_init_uninit();
bool test_show_options();
bool test_word();
// Subtests
void subtest_tscInit(bool& bTestResult);
void subtest_tscCreateSession(tsc_cookie& c, bool& bTestResult);
void subtest_tscDestroySession(tsc_cookie c, bool& bTestResult);
void subtest_tscUninit(bool& bTestResult);

//static char s_szTextResult[64];
static string s_sTextResult;
static int g_nTestStartCount, g_nTestEndCount, g_nSuccessCount, g_nFailCount;
static const char* g_szLine = "--------------------------------------------------------------------------------";

int main()
{
    g_nTestStartCount = g_nTestEndCount = g_nSuccessCount = g_nFailCount = 0;
    
    cout << "Tolon Spell Check Test Program. Copyright (c) 2009 Alex Paterson." << endl;
    cout << g_szLine << endl;
    
    check_test(test_abnormal_init_uninit());
    check_test(test_normal_init_uninit());
    check_test(test_no_init());
    check_test(test_getversion());
    check_test(test_word());
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
    ++g_nTestStartCount;
    cout << szPreamble << endl;
}

void util_end_test(bool bResult)
{
    ++g_nTestEndCount;
    if (bResult)
        cout << "Test ok" << endl;
    else
        cout << "TEST FAILED" << endl;
    cout << g_szLine << endl;
}

void util_is_expected(char* szTestPartName, const tsc_result rExpected, const tsc_result rActual, bool& bSetFalseIfFailed)
{
    if (rActual == rExpected) {
        cout << "\t" << szTestPartName <<" ok" << endl;
    }
    else {
        cout << "FAIL->\t" << szTestPartName <<" FAIL; expected " << util_textify_result(rExpected) << ", got " << util_textify_result(rActual) << endl;
        bSetFalseIfFailed = false; 
    }
    cout << "\t\tLastError: " <<  ::tscGetLastError() << endl;
}

void util_is_failure(char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfNotFailed)
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

void util_is_success(char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfFailed)
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
            s_sTextResult.swap(ss.str());
            return s_sTextResult.c_str();
    }
    
    return "";
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

bool test_getversion()
{
    bool bTestResult = true;
    tsc_result r;
    TSC_INIT_DATA id;
    TSC_VERSION_DATA vd;
    
    util_begin_test("test_getversion()\r\nTesting tscGetVersion...");
    
    memset(&id, 0xff, sizeof(TSC_INIT_DATA));
    id.cbSize = sizeof(TSC_INIT_DATA);
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
    
    r = ::tscGetVersion(NULL);
    util_is_expected("tscGetVersion", rExpected, r, bTestResult);
    
    r = ::tscCreateSession(NULL, NULL);
    util_is_expected("tscCreateSession", rExpected, r, bTestResult);
    
    r = ::tscDestroySession(TSC_NULL_COOKIE);
    util_is_expected("tscDestroySession", rExpected, r, bTestResult);
    
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

bool test_normal_init_uninit()
{
    bool bTestResult = true;
    tsc_result r;
    TSC_INIT_DATA id;
    
    util_begin_test("test_normal_init_uninit()\r\nTesting normal tscInit/tscUninit sequence...");
    
    memset(&id, 0xff, sizeof(TSC_INIT_DATA));
    id.cbSize = sizeof(TSC_INIT_DATA);
    strcpy(id.szAppName, "TestAppName01234567890123456789");
    
    r = ::tscInit(&id);
    util_is_success("tscInit", r, bTestResult);
    
    r = ::tscUninit();
    util_is_success("tscUninit", r, bTestResult);
    
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
    sow.hWndParent = NULL;
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

bool test_word()
{
    bool bTestResult = true;
    tsc_result r;
    tsc_cookie c = TSC_NULL_COOKIE;
    const char* szWordToTest = "helllllo";
    
    util_begin_test("test_word()\r\nTesting tscCheckWord(\"helllo\")...");
    
    // Prologue
    subtest_tscInit(bTestResult);
    subtest_tscCreateSession(c, bTestResult);
    
    // CheckWord
    TSC_CHECKWORD_DATA cw;
    memset(&cw, 0xff, sizeof(TSC_CHECKWORD_DATA));
    cw.cbSize = sizeof(TSC_CHECKWORD_DATA);
    cw.uTestWord.szWord8 = szWordToTest;
    cw.uResultString.szResults8 = NULL;
    cw.nResultStringSize = 0;
    r = ::tscCheckWord(c, &cw);   
    util_is_success("tscCheckWord", r, bTestResult);
    
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
