SETLOCAL
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..
SET SourceDir=%RootDir%\Source
SET InstallerDir=%SourceDir%\Installer\SoupInstaller\msi

pushd %InstallerDir%
start msiexec /package SoupBuild.msi /passive
popd