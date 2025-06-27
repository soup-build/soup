@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..
SET CodeDir=%RootDir%\code

REM - Client Tools
echo soup restore %CodeDir%/client/tools
soup restore %CodeDir%/client/tools
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %CodeDir%/client/tools -flavor %Flavor%
soup build %CodeDir%/client/tools -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Bootstrap
echo soup restore %CodeDir%/tools/bootstrap
soup restore %CodeDir%/tools/bootstrap
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %CodeDir%/tools/bootstrap -flavor %Flavor%
soup build %CodeDir%/tools/bootstrap -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Copy
echo soup restore %CodeDir%/tools/copy
soup restore %CodeDir%/tools/copy
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %CodeDir%/tools/copy -flavor %Flavor%
soup build %CodeDir%/tools/copy -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Mkdir
echo soup restore %CodeDir%/tools/mkdir
soup restore %CodeDir%/tools/mkdir
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %CodeDir%/tools/mkdir -flavor %Flavor%
soup build %CodeDir%/tools/mkdir -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Parse Modules
echo soup restore %CodeDir%/tools/parse-modules
soup restore %CodeDir%/tools/parse-modules
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %CodeDir%/tools/parse-modules -flavor %Flavor%
soup build %CodeDir%/tools/parse-modules -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Print Graph
echo soup restore %CodeDir%/tools/print-graph
soup restore %CodeDir%/tools/print-graph
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %CodeDir%/tools/print-graph -flavor %Flavor%
soup build %CodeDir%/tools/print-graph -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Print Results
echo soup restore %CodeDir%/tools/print-results
soup restore %CodeDir%/tools/print-results
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %CodeDir%/tools/print-results -flavor %Flavor%
soup build %CodeDir%/tools/print-results -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Print ValueTable
echo soup restore %CodeDir%/tools/print-valuetable
soup restore %CodeDir%/tools/print-valuetable
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

echo soup build %CodeDir%/tools/print-valuetable -flavor %Flavor%
soup build %CodeDir%/tools/print-valuetable -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%
