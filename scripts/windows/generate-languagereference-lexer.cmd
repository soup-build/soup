@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..\
SET ClientCoreDir=code\client\core\source
SET REFLEX=%UserProfile%\source\repos\RE-flex\bin\win64\reflex.exe

pushd %RootDir%
echo %REFLEX% %ClientCoreDir%\recipe\language-reference-parser.l -o %ClientCoreDir%\recipe\language-reference-parser.cpp
call %REFLEX% %ClientCoreDir%\recipe\language-reference-parser.l -o %ClientCoreDir%\recipe\language-reference-parser.cpp
popd