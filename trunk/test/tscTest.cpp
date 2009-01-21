#include "TolonSpellCheck.h"
#include <iostream>

using namespace std;

void util_begin_test(char* szPreamble);
void util_end_test(bool bResult);
void util_is_expected(tsc_result r, bool& bSetFalseIfFailed);
void util_is_failure(char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfNotFailed);
void util_is_success(char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfFailed);
void check_test(bool bResult);
void print_stats();
// The tests
bool test_abnormal_init_uninit();
bool test_getversion();
bool test_no_init();
bool test_normal_init_uninit();
bool test_show_options();
bool test_word();

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
    if (!bResult)
        cout << "FAIL" << endl;
    cout << g_szLine << endl;
}

void util_is_expected(char* szTestPartName, const tsc_result rExpected, const tsc_result rActual, bool& bSetFalseIfFailed)
{
    if (rActual == rExpected) {
        cout << "\t" << szTestPartName <<" ok" << endl;
    }
    else {
        cout << "\t" << szTestPartName <<" FAIL; expected 0x" << hex << rExpected << ",got 0x" << hex << rActual << endl;
        bSetFalseIfFailed = false; 
    }
}

void util_is_failure(char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfNotFailed)
{
    if (FAILED(rActual)) {
        cout << "\t" << szTestPartName <<" ok" << endl;
    }
    else {
        cout << "\t" << szTestPartName <<" FAIL; expected failure, got 0x" << hex << rActual << endl;
        bSetFalseIfNotFailed = false; 
    }
}

void util_is_success(char* szTestPartName, const tsc_result rActual, bool& bSetFalseIfFailed)
{
    if (SUCCEEDED(rActual)) {
        cout << "\t" << szTestPartName <<" ok" << endl;
    }
    else {
        cout << "\t" << szTestPartName <<" FAIL; expected success, got 0x" << hex << rActual << endl;
        bSetFalseIfFailed = false; 
    }
}

// Tests below are in alphabetical order

bool test_abnormal_init_uninit()
{
    bool bTestResult = true;
    tsc_result r;
    TSC_INIT_DATA id;
    
    util_begin_test("Testing abnormal tscInit/tscUninit sequences...");
    
    memset(&id, 0xff, sizeof(TSC_INIT_DATA));
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
    tsc_result tr;
    TSC_VERSION_DATA vd;
    
    util_begin_test("Testing tscGetVersion...");
    
    memset(&vd, 0xff, sizeof(TSC_VERSION_DATA));
    
    tr = ::tscGetVersion(&vd);
    
    util_is_success("tscGetVersion", tr, bTestResult);
    
    util_end_test(bTestResult);
    
    return bTestResult;
}

bool test_no_init()
{
    bool bTestResult = true;
    const tsc_result rExpected = E_UNEXPECTED;
    tsc_result r = 0;
    
    util_begin_test("Testing API use without calling tscInit() first...");
    
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
    
    return false;
}

bool test_normal_init_uninit()
{
    bool bTestResult = true;
    tsc_result r;
    TSC_INIT_DATA id;
    
    util_begin_test("Testing normal tscInit/tscUninit sequence...");
    
    memset(&id, 0xff, sizeof(TSC_INIT_DATA));
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
    TSC_INIT_DATA id;
    //TSC_CHECKWORD_DATA cw;
    const char* szWordToTest = "helllllo";
    
    util_begin_test("Testing tscCheckWord(\"helllo\")...");
    
    memset(&id, 0xff, sizeof(TSC_INIT_DATA));
    strcpy(id.szAppName, "TestAppName01234567890123456789");
    r = ::tscInit(&id);
    
    //memset(&cw, 0xff, sizeof(TSC_CHECKWORD_DATA));
    //cw.uTestWord.szWord8 = szWordToTest;
    r = ::tscShowOptionsWindow(TSC_NULL_COOKIE, NULL);   
    util_is_success("tscShowOptionsWindow", r, bTestResult);    
    
    r = ::tscUninit();
    
    util_end_test(bTestResult);
    
    return bTestResult;
}

bool test_word()
{
    bool bTestResult = true;
    tsc_result r;
    TSC_INIT_DATA id;
    TSC_CHECKWORD_DATA cw;
    const char* szWordToTest = "helllllo";
    
    util_begin_test("Testing tscCheckWord(\"helllo\")...");
    
    memset(&id, 0xff, sizeof(TSC_INIT_DATA));
    strcpy(id.szAppName, "TestAppName01234567890123456789");
    r = ::tscInit(&id);
    
    memset(&cw, 0xff, sizeof(TSC_CHECKWORD_DATA));
    cw.uTestWord.szWord8 = szWordToTest;
    r = ::tscCheckWord(TSC_NULL_COOKIE, &cw);   
    util_is_success("tscCheckWord", r, bTestResult);    
    
    r = ::tscUninit();
    
    util_end_test(bTestResult);
    
    return bTestResult;
}