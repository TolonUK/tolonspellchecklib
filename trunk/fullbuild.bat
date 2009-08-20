@echo off
set topdir=%cd%
set gettext-version=0.14.5
set libiconv-version=1.9.1
set glib-version=2.20.4
set gettext-srcball=gettext-%gettext-version%.tar.gz
set libiconv-srcball=libiconv-%libiconv-version%.tar.gz
set glib-srcball=glib-%glib-version%.tar.bz2

:DECOMPRESS
echo Decompressing libraries...
cd .\contrib
echo    decompressing gettext...
rem gunzip -c %gettext-srcball% | tar x
echo    decompressing libiconv...
rem gunzip -c %libiconv-srcball% | tar x
echo    decompressing glib...
rem bunzip2 -c %glib-srcball% | tar x
cd ..

:RENAME
echo Renaming library folder for glib...
cd .\contrib
rename glib-%glib-version% glib
cd ..

:BUILD
mkdir usr

echo Making libiconv (no NLS)...
cd .\contrib\libiconv-%libiconv-version%
nmake -f Makefile.msvc NO_NLS=1 DLL=1 MFLAGS=-MD PREFIX=%topdir%\usr
nmake -f Makefile.msvc NO_NLS=1 DLL=1 MFLAGS=-MD PREFIX=%topdir%\usr install
cd ..\..

echo Making libintl...
cd .\contrib\gettext-%gettext-version%\gettext-runtime
nmake -f Makefile.msvc config.h
cd .\intl
nmake -f Makefile.msvc DLL=1 MFLAGS=-MD PREFIX=%topdir%\usr 
nmake -f Makefile.msvc DLL=1 MFLAGS=-MD PREFIX=%topdir%\usr install
cd ..\..\..

echo Making libiconv (no NLS)...
cd .\contrib\libiconv-%libiconv-version%
nmake -f Makefile.msvc DLL=1 MFLAGS=-MD PREFIX=%topdir%\usr clean
nmake -f Makefile.msvc DLL=1 MFLAGS=-MD PREFIX=%topdir%\usr
nmake -f Makefile.msvc DLL=1 MFLAGS=-MD PREFIX=%topdir%\usr install
cd ..\..

echo Making libglib and libgmodule...
cd .\contrib\glib
nmake -f Makefile.msc config.h glibconfig.h 
cd .\build\win32\dirent
nmake -f makefile.msc INTL=%topdir%\usr
cd ..\..\..\glib
nmake -f Makefile.msc INTL=%topdir%\usr
copy .\glib-2.0.lib %topdir%\usr\lib 
copy .\libglib-2.0-0.dll %topdir%\usr\bin 
cd ..\gmodule
nmake -f Makefile.msc INTL=%topdir%\usr 
copy gmodule-2.0.lib %topdir%\usr\lib 
copy libgmodule-2.0-0.dll %topdir%\usr\bin 
cd ..\..\..

:EXIT
popd
