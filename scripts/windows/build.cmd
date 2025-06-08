@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0

REM - Build Client
echo %ScriptsDir%/build-client.cmd %Flavor%
call %ScriptsDir%/build-client.cmd %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Build SWhere tool
echo %ScriptsDir%/build-swhere.cmd %Flavor%
call %ScriptsDir%/build-swhere.cmd %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Build PackageManager
echo %ScriptsDir%/build-packagemanager.cmd %Flavor%
call %ScriptsDir%/build-packagemanager.cmd %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Build Tools
echo %ScriptsDir%/build-tools.cmd %Flavor%
call %ScriptsDir%/build-tools.cmd %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Build View
echo %ScriptsDir%/build-view.cmd %Flavor%
call %ScriptsDir%/build-view.cmd %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%