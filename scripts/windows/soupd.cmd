@echo off
SETLOCAL
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..
SET OutDir=%RootDir%\out
SET MSBuildDir=%OutDir%\msbuild
SET RunDir=%OutDir%\run

SET ConfigHash=OZlIVjblazFuKXg-raWUNoGEnG4

SET SOUP_VERSION=0.43.1

REM - Cleanup previous runs
rmdir /S /Q %RunDir% > NUL

REM - Use a copy of the final binary in case we are re-building itself
robocopy %ScriptsDir%\install\ %RunDir%\ /MIR /NJH /NJS /NDL > NUL
robocopy %OutDir%\C++\Local\Soup\%SOUP_VERSION%\%ConfigHash%\bin\ %RunDir%\Soup\ /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\soup.build.package-manager\Debug\net10.0\win-x64\publish\ %RunDir%\Soup\package-manager\ /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\soup-view\Debug\net10.0\win-x64\publish\ %RunDir%\Soup\view\ /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\swhere\Debug\net10.0\win-x64\publish\ %RunDir%\ swhere.exe /NJH /NJS /NDL > NUL

%RunDir%\soup.cmd %*