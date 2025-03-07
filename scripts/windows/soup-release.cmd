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

SET ConfigHash=Oltq7cGwk0Rbgy1I-3mCMDDE5yM

SET PKG_OWNER=mwasplund

SET SOUP_VERSION=0.41.4
SET COPY_VERSION=1.1.0
SET MKDIR_VERSION=1.1.0
SET SOUP_WREN_VERSION=0.4.3

REM - Use a copy of the final binary in case we are re-building itself
robocopy %ScriptsDir%\install\ %RunDir%\ /MIR /NJH /NJS /NDL > NUL
robocopy %OutDir%\C++\Local\Soup\%SOUP_VERSION%\%ConfigHash%\bin\ %RunDir%\Soup\ /MIR /NJH /NJS /NDL > NUL

robocopy %CodeDir%\tools\copy\ %RunDir%\Soup\BuiltIn\%PKG_OWNER%\copy\%COPY_VERSION%\ Recipe.sml /NJH /NJS /NDL > NUL
robocopy %OutDir%\C++\Local\copy\%COPY_VERSION%\%ConfigHash%\ %RunDir%\Soup\BuiltIn\%PKG_OWNER%\copy\%COPY_VERSION%\out\ /MIR /NJH /NJS /NDL > NUL

robocopy %CodeDir%\tools\mkdir\ %RunDir%\Soup\BuiltIn\%PKG_OWNER%\mkdir\%MKDIR_VERSION%\ Recipe.sml /NJH /NJS /NDL > NUL
robocopy %OutDir%\C++\Local\mkdir\%MKDIR_VERSION%\%ConfigHash%\ %RunDir%\Soup\BuiltIn\%PKG_OWNER%\mkdir\%MKDIR_VERSION%\out\ /MIR /NJH /NJS /NDL > NUL

robocopy %GlobalPackagesDir%\Wren\Soup\Wren\%SOUP_WREN_VERSION%\ %RunDir%\Soup\BuiltIn\Soup\Wren\%SOUP_WREN_VERSION%\ Recipe.sml /NJH /NJS /NDL > NUL
robocopy %GlobalOutDir%\Wren\Soup\Wren\%SOUP_WREN_VERSION%\%ConfigHash%\ %RunDir%\Soup\BuiltIn\Soup\Wren\%SOUP_WREN_VERSION%\out\ /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\Soup.Build.PackageManager\Release\net9.0\win-x64\publish\ %RunDir%\Soup\PackageManager\ *.dll *.exe /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\SoupView\Release\net9.0\win-x64\publish\ %RunDir%\Soup\View\ *.dll *.exe /MIR /NJH /NJS /NDL > NUL

robocopy %MSBuildDir%\bin\Swhere\Release\net9.0\win-x64\publish\ %RunDir%\ swhere.exe /NJH /NJS /NDL > NUL

%RunDir%\soup.cmd %*