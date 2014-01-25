mkdir %1\usr-debug
mkdir %1\usr-release
xcopy /e /y .\usr-debug %1\usr-debug
xcopy /e /y .\usr-release %1\usr-release

mkdir %1\usr-debug\bin\enchant
mkdir %1\usr-release\bin\enchant
del %1\usr-debug\bin\enchant\libenchant_*.dll
move %1\usr-debug\bin\libenchant_*.dll %1\usr-debug\bin\enchant
del %1\usr-release\bin\enchant\libenchant_*.dll
move %1\usr-release\bin\libenchant_*.dll %1\usr-release\bin\enchant
