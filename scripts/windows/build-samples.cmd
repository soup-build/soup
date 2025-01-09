@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%../..
SET SamplesDir=%RootDir%/samples

REM - ########################################
REM - ## C Samples
REM - ########################################

echo soup restore %SamplesDir%/c/build-extension/executable/
call soup restore %SamplesDir%/c/build-extension/executable/
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c/build-extension/executable/ -flavor %Flavor%
call soup build %SamplesDir%/c/build-extension/executable/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c/console-application/ -flavor %Flavor%
call soup build %SamplesDir%/c/console-application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c/dynamic-library/application/ -flavor %Flavor%
call soup build %SamplesDir%/c/dynamic-library/application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c/static-library/application/ -flavor %Flavor%
call soup build %SamplesDir%/c/static-library/application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c/windows-application/ -flavor %Flavor%
call soup build %SamplesDir%/c/windows-application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - ########################################
REM - ## C# Samples
REM - ########################################

echo soup restore %SamplesDir%/c#/build-extension/executable/
call soup restore %SamplesDir%/c#/build-extension/executable/
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c#/build-extension/executable/ -flavor %Flavor%
call soup build %SamplesDir%/c#/build-extension/executable/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c#/console-application/ -flavor %Flavor%
call soup build %SamplesDir%/c#/console-application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c#/library/application/ -flavor %Flavor%
call soup build %SamplesDir%/c#/library/application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - ########################################
REM - ## C++ Samples
REM - ########################################

echo soup restore %SamplesDir%/c++/build-extension/executable/
call soup restore %SamplesDir%/c++/build-extension/executable/
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c++/build-extension/executable/ -flavor %Flavor%
call soup build %SamplesDir%/c++/build-extension/executable/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c++/console-application/ -flavor %Flavor%
call soup build %SamplesDir%/c++/console-application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c++/directx/ -flavor %Flavor%
call soup build %SamplesDir%/c++/directx/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c++/dynamic-library/application/ -flavor %Flavor%
call soup build %SamplesDir%/c++/dynamic-library/application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c++/header-library/application/ -flavor %Flavor%
call soup build %SamplesDir%/c++/header-library/application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c++/module-dynamic-library/application/ -flavor %Flavor%
call soup build %SamplesDir%/c++/module-dynamic-library/application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c++/module-interface/ -flavor %Flavor%
call soup build %SamplesDir%/c++/module-interface/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup restore %SamplesDir%/c++/parse-json/
call soup restore %SamplesDir%/c++/parse-json/
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c++/parse-json/ -flavor %Flavor%
call soup build %SamplesDir%/c++/parse-json/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c++/static-library/application/ -flavor %Flavor%
call soup build %SamplesDir%/c++/static-library/application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %SamplesDir%/c++/windows-application/ -flavor %Flavor%
call soup build %SamplesDir%/c++/windows-application/ -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%