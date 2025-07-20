@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..
SET CodeDir=%RootDir%\code
SET OutputDir=%RootDir%\out
SET ClientCLIDir=%CodeDir%\client\cli
SET MonitorClientDir=%CodeDir%\monitor\client

REM - Restore client cli
echo soup restore %ClientCLIDir%
call soup restore %ClientCLIDir%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Build each version of the monitor client dll
echo soup build %MonitorClientDir% -architecture x64 -flavor %Flavor%
call soup build %MonitorClientDir% -architecture x64 -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%
echo soup build %MonitorClientDir% -architecture x86 -flavor %Flavor%
call soup build %MonitorClientDir% -architecture x86 -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Build client cli
echo soup build %ClientCLIDir% -flavor %Flavor%
call soup build %ClientCLIDir% -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Get the targets
for /f %%i in ('soup target %ClientCLIDir% -flavor %Flavor%') do set ClientCLIOutputDirectory=%%i
for /f %%i in ('soup target %MonitorClientDir% -architecture x64 -flavor %Flavor%') do set MonitorClientOutputX64Directory=%%i
for /f %%i in ('soup target %MonitorClientDir% -architecture x86 -flavor %Flavor%') do set MonitorClientOutputX86Directory=%%i

REM - Copy the monitor client dlls
echo copy "%MonitorClientOutputX64Directory%\bin\Monitor.Client.dll" "%ClientCLIOutputDirectory%\bin\Monitor.Client.64.dll"
copy "%MonitorClientOutputX64Directory%\bin\Monitor.Client.dll" "%ClientCLIOutputDirectory%\bin\Monitor.Client.64.dll"
echo copy "%MonitorClientOutputX86Directory%\bin\Monitor.Client.dll" "%ClientCLIOutputDirectory%\bin\Monitor.Client.32.dll"
copy "%MonitorClientOutputX86Directory%\bin\Monitor.Client.dll" "%ClientCLIOutputDirectory%\bin\Monitor.Client.32.dll"

REM - Restore client native
echo soup restore %CodeDir%/client/native
soup restore %CodeDir%/client/native
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%

REM - Build client native
echo soup build %CodeDir%/client/native -flavor %Flavor%
soup build %CodeDir%/client/native -flavor %Flavor%
if %ERRORLEVEL% NEQ  0 exit /B %ERRORLEVEL%