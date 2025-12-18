@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..\
SET ParseModuleDir=code\tools\parse-modules
SET REFLEX=S:\repos\RE-flex\bin\win64\reflex.exe

pushd %RootDir%
echo %REFLEX% %ParseModuleDir%\parser\module-parser.l -o %ParseModuleDir%\parser\module-parser.cpp
call %REFLEX% %ParseModuleDir%\parser\module-parser.l -o %ParseModuleDir%\parser\module-parser.cpp
popd