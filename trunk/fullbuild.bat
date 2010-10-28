@echo off

set topdir=%cd%
set gettext-version=0.14.5
set libiconv-version=1.9.1
set glib-version=2.18.3
set enchant-version=29322
set gettext-srcball=gettext-%gettext-version%.tar.gz
set libiconv-srcball=libiconv-%libiconv-version%.tar.gz
set glib-srcball=glib-%glib-version%.tar.bz2
set enchant-srcball=enchant-%enchant-version%.tar.gz

:OPTIONS
IF /I "%1"=="debug" GOTO OPT_DEBUG

:OPT_RELEASE
set debug=0
set glib_debug=0
set usrdir=%topdir%\usr-release
set mflags=-MD
GOTO OPTIONS_CLEAN

:OPT_DEBUG
set debug=1
set glib_debug=1
set usrdir=%topdir%\usr-debug
set mflags=-MDd
GOTO OPTIONS_CLEAN

:OPT_CLEAN
cd %topdir%
del /s /q .\contrib\libiconv-%libiconv-version%\*.*
rmdir .\contrib\libiconv-%libiconv-version%
del /s /q .\contrib\gettext-%gettext-version%\*.*
rmdir .\contrib\gettext-%gettext-version%
del /s /q .\contrib\glib\*.*
rmdir .\contrib\glib
del /s /q .\contrib\enchant-%enchant-version%\*.*
rmdir .\contrib\enchant-%enchant-version%
GOTO DECOMPRESS

:OPTIONS_CLEAN
IF /I "%2"=="clean" GOTO OPT_CLEAN

:DECOMPRESS
echo Decompressing libraries...
cd .\contrib
echo    decompressing gettext...
gzip -d -c %gettext-srcball% | tar -x --unlink-first --dereference
echo    decompressing libiconv...
gzip -d -c %libiconv-srcball% | tar x
echo    decompressing glib...
bzip2 -d -c %glib-srcball% | tar x
echo    decompressing enchant...
gzip -d -c %enchant-srcball% | tar x
cd ..

:RENAME
echo Renaming library folder for glib...
cd %topdir%
cd .\contrib
rename glib-%glib-version% glib
cd ..

:BUILD
mkdir %usrdir%
mkdir %usrdir%\bin
mkdir %usrdir%\include
mkdir %usrdir%\lib

:BUILD_LIBICONV_FIRST
echo Making libiconv (no NLS)...
cd %topdir%
cd .\contrib\libiconv-%libiconv-version%
nmake -f Makefile.msvc NO_NLS=1 DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir% clean
nmake -f Makefile.msvc NO_NLS=1 DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir%
nmake -f Makefile.msvc NO_NLS=1 DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir% install
cd ..\..

:BUILD_LIBINTL
echo Making libintl...
cd %topdir%
cd .\contrib\gettext-%gettext-version%\gettext-runtime
nmake -f Makefile.msvc config.h
cd .\intl
copy %topdir%\contrib\localename-msvc9.diff .
patch -i localename-msvc9.diff
nmake -f Makefile.msvc DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir% clean
nmake -f Makefile.msvc DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir% 
nmake -f Makefile.msvc DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir% install
mt -manifest %topdir%\contrib\gettext-%gettext-version%\gettext-runtime\intl\intl.dll.manifest -outputresource:%usrdir%\bin\intl.dll;#1

:BUILD_LIBICONV_SECOND
echo Making libiconv (NLS)...
cd %topdir%
cd .\contrib\libiconv-%libiconv-version%
nmake -f Makefile.msvc DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir% clean
nmake -f Makefile.msvc DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir%
nmake -f Makefile.msvc DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir% install
mt -manifest %topdir%\contrib\libiconv-%libiconv-version%\libcharset\lib\charset.dll.manifest -outputresource:%usrdir%\bin\charset.dll;#1
mt -manifest %topdir%\contrib\libiconv-%libiconv-version%\lib\iconv.dll.manifest -outputresource:%usrdir%\bin\iconv.dll;#1
mt -manifest %topdir%\contrib\libiconv-%libiconv-version%\src\iconv.exe.manifest -outputresource:%usrdir%\bin\iconv.exe;#1

:BUILD_LIBGLIB
echo Making libglib and libgmodule...
if %glib_debug% == 0 set debug=
cd %topdir%
cd .\contrib\glib
nmake -f Makefile.msc config.h glibconfig.h 
cd .\build\win32\dirent
nmake -f makefile.msc INTL=%usrdir%
cd ..\..\..\glib
nmake -f Makefile.msc INTL=%usrdir% clean
nmake -f Makefile.msc INTL=%usrdir%
mt -manifest libglib-2.0-0.dll.manifest -outputresource:libglib-2.0-0.dll;#1
copy .\glib-2.0.lib %usrdir%\lib 
copy .\libglib-2.0-0.dll %usrdir%\bin 
cd ..\gmodule
nmake -f Makefile.msc INTL=%usrdir% clean
nmake -f Makefile.msc INTL=%usrdir% 
mt -manifest libgmodule-2.0-0.dll.manifest -outputresource:libgmodule-2.0-0.dll;#1
copy gmodule-2.0.lib %usrdir%\lib 
copy libgmodule-2.0-0.dll %usrdir%\bin 
set debug=%glib_debug%

:BUILD_ENCHANT
echo Making enchant...
cd %topdir%
cd .\contrib\enchant-%enchant-version%\src
copy /Y %topdir%\contrib\enchant.c.patch
patch -i enchant.c.patch
cd .\myspell
copy /Y %topdir%\contrib\dictmgr.diff .
copy /Y %topdir%\contrib\myspell_checker.diff .
patch -i dictmgr.diff
patch -i myspell_checker.diff
cd ..\..\msvc
copy /Y %topdir%\contrib\enchant-makefile.msvc makefile.msvc
set cmd=nmake -f Makefile.msvc DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir% GLIBDIR=%topdir%\contrib\glib MANIFEST=1
%cmd% clean
%cmd%
%cmd% install
set cmd=
cd ..\..\..

:BUILD_TOLONSPELLCHECK
echo Making tolonspellchecklib...
cd %topdir%
set cmd=nmake -f Makefile.msvc DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir% GLIBDIR=%topdir%\contrib\glib MANIFEST=1
%cmd% clean
%cmd%
%cmd% install
set cmd=

:EXIT
cd %topdir%
