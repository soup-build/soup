@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..
SET CodeDir=%RootDir%\code
SET ToolsDir=%CodeDir%\client\tools

REM - Restore Tools
echo soup restore %ToolsDir%
call soup restore %ToolsDir%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Build Tools
echo soup build %ToolsDir% -flavor %Flavor%
call soup build %ToolsDir% -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%
