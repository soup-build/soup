SETLOCAL
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..
SET OutDir=%RootDir%\out

SET SOUP_VERSION=0.43.1

pushd %OutDir%\release\%SOUP_VERSION%
msiexec /package soup-build-windows-x64.msi /passive
popd