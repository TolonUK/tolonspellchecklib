set debug=1
set mflags=-MDd
set usrdir=%cd%\usr-debug
set cmd=nmake -nologo -s -f Makefile.msvc DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir% GLIBDIR=%cd%\contrib\glib MANIFEST=1
%cmd% clean
%cmd%
%cmd% install
set cmd=
