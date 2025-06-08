@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..\
SET ParseModuleDir=code\tools\parse-modules
SET REFLEX=%UserProfile%\source\repos\RE-flex\bin\win64\reflex.exe

pushd %RootDir%
echo %REFLEX% %ParseModuleDir%\parser\ModuleParser.l -o %ParseModuleDir%\parser\ModuleParser.cpp
call %REFLEX% %ParseModuleDir%\parser\ModuleParser.l -o %ParseModuleDir%\parser\ModuleParser.cpp
popd