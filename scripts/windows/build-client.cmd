@echo off
SETLOCAL
SET Flavor=%1
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..
SET CodeDir=%RootDir%\code
SET OutputDir=%RootDir%\out
SET ClientCLIDir=%CodeDir%\client\cli
SET MonitorClientDir=%CodeDir%\monitor\client

@REM REM - Restore client cli
@REM echo soup restore %ClientCLIDir%
@REM call soup restore %ClientCLIDir%
@REM if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

@REM REM - Build each version of the monitor client dll
@REM echo soup build %MonitorClientDir% -architecture x64 -flavor %Flavor%
@REM call soup build %MonitorClientDir% -architecture x64 -flavor %Flavor%
@REM if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%
@REM echo soup build %MonitorClientDir% -architecture x86 -flavor %Flavor%
@REM call soup build %MonitorClientDir% -architecture x86 -flavor %Flavor%
@REM if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

@REM REM - Build client cli
@REM echo soup build %ClientCLIDir% -flavor %Flavor%
@REM call soup build %ClientCLIDir% -flavor %Flavor%
@REM if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%

@REM REM - Get the targets
@REM for /f %%i in ('soup target %ClientCLIDir% -flavor %Flavor%') do set ClientCLIOutputDirectory=%%i
@REM for /f %%i in ('soup target %MonitorClientDir% -architecture x64 -flavor %Flavor%') do set MonitorClientOutputX64Directory=%%i
@REM for /f %%i in ('soup target %MonitorClientDir% -architecture x86 -flavor %Flavor%') do set MonitorClientOutputX86Directory=%%i

@REM REM - Copy the monitor client dlls
@REM echo copy "%MonitorClientOutputX64Directory%\bin\Monitor.Client.dll" "%ClientCLIOutputDirectory%\bin\Monitor.Client.64.dll"
@REM copy "%MonitorClientOutputX64Directory%\bin\Monitor.Client.dll" "%ClientCLIOutputDirectory%\bin\Monitor.Client.64.dll"
@REM echo copy "%MonitorClientOutputX86Directory%\bin\Monitor.Client.dll" "%ClientCLIOutputDirectory%\bin\Monitor.Client.32.dll"
@REM copy "%MonitorClientOutputX86Directory%\bin\Monitor.Client.dll" "%ClientCLIOutputDirectory%\bin\Monitor.Client.32.dll"

REM - Build client native
echo soup build %CodeDir%\client\native -flavor %Flavor%
call soup build %CodeDir%\client\native -flavor %Flavor%
if %ERRORLEVEL% NEQ 0 exit /B %ERRORLEVEL%