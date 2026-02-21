@echo off
SETLOCAL
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..
SET OutDir=%RootDir%\out
SET MSBuildDir=%OutDir%\msbuild
SET RunDir=%OutDir%\run

SET ConfigHash=Oltq7cGwk0Rbgy1I-3mCMDDE5yM

SET SOUP_VERSION=0.43.1

REM - Use a copy of the final binary in case we are re-building itself
robocopy %ScriptsDir%\install\ %RunDir%\ /MIR /NJH /NJS /NDL > NUL
robocopy %OutDir%\C++\Local\Soup\%SOUP_VERSION%\%ConfigHash%\bin\ %RunDir%\Soup\ /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\soup.build.package-manager\Release\net10.0\win-x64\publish\ %RunDir%\Soup\PackageManager\ *.dll *.exe /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\soup-view\Release\net10.0\win-x64\publish\ %RunDir%\Soup\View\ *.dll *.exe /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\swhere\Release\net10.0\win-x64\publish\ %RunDir%\ swhere.exe /NJH /NJS /NDL > NUL

%RunDir%\soup.cmd %*