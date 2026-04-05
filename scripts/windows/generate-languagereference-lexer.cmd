@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..\
SET SMLDir=code\sml
SET REFLEX=%UserProfile%\source\repos\RE-flex\bin\win64\reflex.exe

pushd %RootDir%
echo %REFLEX% %SMLDir%\language-reference-parser.l -o %SMLDir%\language-reference-parser.cpp
call %REFLEX% %SMLDir%\language-reference-parser.l -o %SMLDir%\language-reference-parser.cpp
popd