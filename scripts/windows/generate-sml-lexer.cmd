@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..\
SET ClientCoreDir=code\client\core\source
SET REFLEX=%UserProfile%\source\repos\RE-flex\bin\win64\reflex.exe

pushd %RootDir%
echo %REFLEX% %ClientCoreDir%\sml\sml-parser.l -o %ClientCoreDir%\sml\sml-parser.cpp
call %REFLEX% %ClientCoreDir%\sml\sml-parser.l -o %ClientCoreDir%\sml\sml-parser.cpp
popd