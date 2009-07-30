#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>
#include <vector>

#include <windows.h>

#define BOOST_UTF8_BEGIN_NAMESPACE
#define BOOST_UTF8_END_NAMESPACE
#define BOOST_UTF8_DECL
#include "libs/detail/utf8_codecvt_facet.cpp"

using namespace std;

void wmain()
{
    //BOOL bCP = ::SetConsoleOutputCP(65001);
    BOOL bCP = ::SetConsoleOutputCP(1200);

    wcout << L"Test" << endl;

    locale old_locale;
    locale utf8_locale(old_locale, new utf8_codecvt_facet);

    // Set a New global locale
    locale::global(utf8_locale);

    std::vector<wchar_t> from_file;
    std::wifstream ifs("utf8sample.txt");
    ifs.imbue(utf8_locale);

    //wcout.imbue(old_locale);
    wchar_t wch = 0;
    while (ifs.get(wch)) from_file.push_back(wch);
  
    /*from_file.push_back(L'\0');
    OutputDebugString(&(*from_file.begin()));*/

    std::vector<wchar_t> word;
    std::vector<wchar_t>::const_iterator it;
    for ( it = from_file.begin();
          it != from_file.end();
          ++it )
    {
        if (std::isalpha(*it, utf8_locale))
        {
            word.push_back(*it);
        }
        else
        {
            if (word.empty() == false)
            {
                word.push_back(L'\0');
                OutputDebugString(&(*word.begin()));
                OutputDebugString(L"\r\n");
                word.clear();
            }
        }
    }

    if (word.empty() == false)
    {
        word.push_back(L'\0');
        OutputDebugString(&(*word.begin()));
        OutputDebugString(L"\r\n");
        word.clear();
    }

    /*std::vector<wchar_t>::const_iterator it;
    for ( it = from_file.begin();
          it != from_file.end();
          ++it )
    {
        wcout << *it;
    }
    wcout.flush();*/

    //copy(from_file.begin(), from_file.end(), std::ostream_iterator<wchar_t, wchar_t>(std::wcout));
}