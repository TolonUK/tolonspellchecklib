mkdir %1\usr-debug
mkdir %1\usr-release
xcopy /e /y .\usr-debug %1\usr-debug
xcopy /e /y .\usr-release %1\usr-release

rem mkdir %1\usr-debug\bin\enchant
rem mkdir %1\usr-release\bin\enchant
rem del %1\usr-debug\bin\enchant\libenchant_*.dll
rem move %1\usr-debug\bin\libenchant_*.dll %1\usr-debug\bin\enchant
rem del %1\usr-release\bin\enchant\libenchant_*.dll
rem move %1\usr-release\bin\libenchant_*.dll %1\usr-release\bin\enchant

mkdir %1\usr-debug\data\myspell
mkdir %1\usr-release\data\myspell
xcopy /e /q /y .\data\core_dicts\myspell\*.* %1\usr-debug\data\myspell
xcopy /e /q /y .\data\core_dicts\myspell\*.* %1\usr-release\data\myspell
