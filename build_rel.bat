set debug=0
set mflags=-MD
set usrdir=%cd%\usr-release
set cmd=nmake -nologo -f Makefile.msvc DLL=1 MFLAGS=%mflags% DEBUG=%debug% PREFIX=%usrdir% GLIBDIR=%cd%\contrib\glib MANIFEST=1
%cmd% clean
%cmd%
%cmd% install
set cmd=
