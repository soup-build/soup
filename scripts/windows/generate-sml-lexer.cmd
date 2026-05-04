@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..\
SET SMLDir=code\sml\
SET REFLEX=%UserProfile%\source\repos\RE-flex\bin\win64\reflex.exe

pushd %RootDir%
echo %REFLEX% %SMLDir%\sml-parser.l -o %SMLDir%\sml-parser.cpp
call %REFLEX% %SMLDir%\sml-parser.l -o %SMLDir%\sml-parser.cpp
popd