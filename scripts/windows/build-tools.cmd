@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..
SET CodeDir=%RootDir%\code
SET ToolsDir=%CodeDir%\tools

REM - Bootstrap
echo soup restore %ToolsDir%\bootstrap
call soup restore %ToolsDir%\bootstrap
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %ToolsDir%\bootstrap -flavor %Flavor%
call soup build %ToolsDir%\bootstrap -flavor %Flavor%
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

REM - Copy
echo soup restore %ToolsDir%\copy
call soup restore %ToolsDir%\copy
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %ToolsDir%\copy -flavor %Flavor%
call soup build %ToolsDir%\copy -flavor %Flavor%
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

REM - Mkdir
echo soup restore %ToolsDir%\mkdir
call soup restore %ToolsDir%\mkdir
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %ToolsDir%\mkdir -flavor %Flavor%
call soup build %ToolsDir%\mkdir -flavor %Flavor%
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

REM - Parse Modules
echo soup restore %ToolsDir%\parse-modules
call soup restore %ToolsDir%\parse-modules
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %ToolsDir%\parse-modules -flavor %Flavor%
call soup build %ToolsDir%\parse-modules -flavor %Flavor%
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

REM - Print Graph
echo soup restore %ToolsDir%\print-graph
call soup restore %ToolsDir%\print-graph
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %ToolsDir%\print-graph -flavor %Flavor%
call soup build %ToolsDir%\print-graph -flavor %Flavor%
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

REM - Print Results
echo soup restore %ToolsDir%\print-results
call soup restore %ToolsDir%\print-results
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %ToolsDir%\print-results -flavor %Flavor%
call soup build %ToolsDir%\print-results -flavor %Flavor%
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

REM - Print ValueTable
echo soup restore %ToolsDir%\print-valuetable
call soup restore %ToolsDir%\print-valuetable
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

echo soup build %ToolsDir%\print-valuetable -flavor %Flavor%
call soup build %ToolsDir%\print-valuetable -flavor %Flavor%
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%