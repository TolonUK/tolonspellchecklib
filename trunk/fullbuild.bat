@echo off

set topdir=%cd%
set gettext-version=0.14.5
set libiconv-version=1.9.1
set glib-version=2.18.3
set enchant-version=27782
set gettext-srcball=gettext-%gettext-version%.tar.gz
set libiconv-srcball=libiconv-%libiconv-version%.tar.gz
set glib-srcball=glib-%glib-version%.tar.bz2
set enchant-srcball=enchant-%enchant-version%.tar.gz

:OPTIONS
IF /I "%1"=="debug" GOTO OPT_DEBUG

:OPT_RELEASE
set debug=0
set usrdir=%topdir%\usr-release
GOTO DECOMPRESS

:OPT_DEBUG
set debug=1
set usrdir=%topdir%\usr-debug
GOTO DECOMPRESS

:DECOMPRESS
echo Decompressing libraries...
cd .\contrib
echo    decompressing gettext...
gzip -d -c %gettext-srcball% | tar x
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

echo Making libiconv (no NLS)...
cd %topdir%
cd .\contrib\libiconv-%libiconv-version%
nmake -f Makefile.msvc NO_NLS=1 DLL=1 MFLAGS=-MD DEBUG=%debug% PREFIX=%usrdir% clean
nmake -f Makefile.msvc NO_NLS=1 DLL=1 MFLAGS=-MD DEBUG=%debug% PREFIX=%usrdir%
nmake -f Makefile.msvc NO_NLS=1 DLL=1 MFLAGS=-MD DEBUG=%debug% PREFIX=%usrdir% install
cd ..\..

echo Making libintl...
cd %topdir%
cd .\contrib\gettext-%gettext-version%\gettext-runtime
nmake -f Makefile.msvc config.h
cd .\intl
nmake -f Makefile.msvc DLL=1 MFLAGS=-MD DEBUG=%debug% PREFIX=%usrdir% clean
nmake -f Makefile.msvc DLL=1 MFLAGS=-MD DEBUG=%debug% PREFIX=%usrdir% 
nmake -f Makefile.msvc DLL=1 MFLAGS=-MD DEBUG=%debug% PREFIX=%usrdir% install
cd ..\..\..\..

echo Making libiconv (no NLS)...
cd %topdir%
cd .\contrib\libiconv-%libiconv-version%
nmake -f Makefile.msvc DLL=1 MFLAGS=-MD DEBUG=%debug% PREFIX=%usrdir% clean
nmake -f Makefile.msvc DLL=1 MFLAGS=-MD DEBUG=%debug% PREFIX=%usrdir%
nmake -f Makefile.msvc DLL=1 MFLAGS=-MD DEBUG=%debug% PREFIX=%usrdir% install
cd ..\..

echo Making libglib and libgmodule...
cd %topdir%
cd .\contrib\glib
nmake -f Makefile.msc config.h glibconfig.h 
cd .\build\win32\dirent
nmake -f makefile.msc INTL=%usrdir%
cd ..\..\..\glib
nmake -f Makefile.msc INTL=%usrdir% clean
nmake -f Makefile.msc INTL=%usrdir%
copy .\glib-2.0.lib %usrdir%\lib 
copy .\libglib-2.0-0.dll %usrdir%\bin 
cd ..\gmodule
nmake -f Makefile.msc INTL=%usrdir% clean
nmake -f Makefile.msc INTL=%usrdir% 
copy gmodule-2.0.lib %usrdir%\lib 
copy libgmodule-2.0-0.dll %usrdir%\bin 
cd ..\..\..

echo Making enchant...
cd %topdir%
cd .\contrib\enchant-%enchant-version%\src\myspell
copy /Y %topdir%\contrib\dictmgr.diff .
copy /Y %topdir%\contrib\myspell_checker.diff .
patch -i dictmgr.diff
patch -i myspell_checker.diff
cd ..\..\msvc
copy /Y %topdir%\contrib\enchant-makefile.msvc makefile.msvc
set cmd=nmake -f Makefile.msvc DLL=1 MFLAGS=-MD DEBUG=%debug% PREFIX=%usrdir% GLIBDIR=%topdir%\contrib\glib MANIFEST=0
%cmd% clean
%cmd%
%cmd% install
set cmd=
cd ..\..\..

:EXIT
cd %topdir%
