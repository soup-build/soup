@echo off
SETLOCAL
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..
SET OutDir=%RootDir%\out
SET RunDir=%OutDir%\run

REM - Use a copy of the final binary in case we are re-buiding itself
robocopy %ScriptsDir%\Install\ %RunDir%\ /MIR /NJH /NJS /NDL > NUL
robocopy %OutDir%\Cpp\Soup\0.29.0\txTMowfPh1V3rPmbvNBmBW9Z8Jg\bin\ %RunDir%\Soup\ /MIR /NJH /NJS /NDL > NUL
robocopy %OutDir%\msbuild\bin\Soup.Build.Generate\Release\net6.0\win-x64\publish\ %RunDir%\Soup\Generate\ /MIR /NJH /NJS /NDL > NUL
robocopy C:\Users\mwasp\Dev\Repos\SoupCpp\Source\ %RunDir%\Soup\Extensions\Soup.Cpp\0.4.3\ /MIR /NJH /NJS /NDL > NUL
robocopy C:\Users\mwasp\Dev\Repos\SoupCSharp\Source\ %RunDir%\Soup\Extensions\Soup.CSharp\0.7.4\ /MIR /NJH /NJS /NDL > NUL
robocopy C:\Users\mwasp\Dev\Repos\SoupWren\Source\ %RunDir%\Soup\Extensions\Soup.Wren\0.1.0\ /MIR /NJH /NJS /NDL > NUL
robocopy %OutDir%\msbuild\bin\Soup.Build.PackageManager\Release\net6.0-windows10.0.17763.0\win-x64\publish\ %RunDir%\Soup\PackageManager\ /MIR /NJH /NJS /NDL > NUL
robocopy %OutDir%\msbuild\bin\Swhere\Release\net6.0\win-x64\publish\ %RunDir%\ swhere.exe /NJH /NJS /NDL > NUL
%RunDir%\Soup.cmd %*