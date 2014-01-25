mkdir .\..\..\notekeeper\trunk\usr-debug
mkdir .\..\..\notekeeper\trunk\usr-release
xcopy /e /y .\usr-debug .\..\..\notekeeper\trunk\usr-debug
xcopy /e /y .\usr-release .\..\..\notekeeper\trunk\usr-release

mkdir .\..\..\notekeeper\trunk\usr-debug\bin\enchant
mkdir .\..\..\notekeeper\trunk\usr-release\bin\enchant
move .\..\..\notekeeper\trunk\usr-debug\bin\libenchant_*.dll .\..\..\notekeeper\trunk\usr-debug\bin\enchant
move .\..\..\notekeeper\trunk\usr-release\bin\libenchant_*.dll .\..\..\notekeeper\trunk\usr-release\bin\enchant
