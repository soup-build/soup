@echo off
SETLOCAL
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..
SET OutDir=%RootDir%\out
SET MSBuildDir=%OutDir%\msbuild
SET RunDir=%OutDir%\run
SET CodeDir=%RootDir%\code
SET GlobalPackagesDir=%UserProfile%\.soup\packages
SET GlobalOutDir=%UserProfile%\.soup\out

SET ConfigHash=OZlIVjblazFuKXg-raWUNoGEnG4

SET PKG_OWNER=mwasplund

SET SOUP_VERSION=0.43.0
SET COPY_VERSION=1.2.0
SET MKDIR_VERSION=1.2.0
SET PARSE_MODULES_VERSION=1.2.0
SET SOUP_WREN_VERSION=0.5.4

REM - Cleanup previous runs
rmdir /S /Q %RunDir% > NUL

REM - Use a copy of the final binary in case we are re-building itself
robocopy %ScriptsDir%\Install\ %RunDir%\ /MIR /NJH /NJS /NDL > NUL
robocopy %OutDir%\C++\Local\Soup\%SOUP_VERSION%\%ConfigHash%\bin\ %RunDir%\Soup\ /MIR /NJH /NJS /NDL > NUL

robocopy %CodeDir%\tools\copy\ %RunDir%\Soup\built-in\%PKG_OWNER%\copy\%COPY_VERSION%\ recipe.sml /NJH /NJS /NDL > NUL
robocopy %OutDir%\C++\Local\copy\%COPY_VERSION%\%ConfigHash%\ %RunDir%\Soup\built-in\%PKG_OWNER%\copy\%COPY_VERSION%\out\ /MIR /NJH /NJS /NDL > NUL

robocopy %CodeDir%\tools\mkdir\ %RunDir%\Soup\built-in\%PKG_OWNER%\mkdir\%MKDIR_VERSION%\ recipe.sml /NJH /NJS /NDL > NUL
robocopy %OutDir%\C++\Local\mkdir\%MKDIR_VERSION%\%ConfigHash%\ %RunDir%\Soup\built-in\%PKG_OWNER%\mkdir\%MKDIR_VERSION%\out\ /MIR /NJH /NJS /NDL > NUL

robocopy %CodeDir%\tools\parse-modules\ %RunDir%\Soup\built-in\%PKG_OWNER%\parse.modules\%PARSE_MODULES_VERSION%\ recipe.sml /NJH /NJS /NDL > NUL
robocopy %OutDir%\C++\Local\parse.modules\%PARSE_MODULES_VERSION%\%ConfigHash%\ %RunDir%\Soup\built-in\%PKG_OWNER%\parse.modules\%PARSE_MODULES_VERSION%\out\ /MIR /NJH /NJS /NDL > NUL

robocopy %GlobalPackagesDir%\Wren\Soup\Wren\%SOUP_WREN_VERSION%\ %RunDir%\Soup\built-in\Soup\Wren\%SOUP_WREN_VERSION%\ recipe.sml /NJH /NJS /NDL > NUL
robocopy %GlobalOutDir%\Wren\Soup\Wren\%SOUP_WREN_VERSION%\%ConfigHash%\ %RunDir%\Soup\built-in\Soup\Wren\%SOUP_WREN_VERSION%\out\ /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\soup.build.package-manager\Debug\net10.0\win-x64\publish\ %RunDir%\Soup\package-manager\ /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\soup-view\Debug\net10.0\win-x64\publish\ %RunDir%\Soup\view\ /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\swhere\Debug\net10.0\win-x64\publish\ %RunDir%\ swhere.exe /NJH /NJS /NDL > NUL

%RunDir%\Soup.cmd %*