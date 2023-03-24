@echo off
SETLOCAL
SET ScriptsDir=%~dp0
SET RootDir=%ScriptsDir%..\..
SET SourceDir=%RootDir%\Source
SET InstallerDir=%SourceDir%\Installer\SoupInstaller\msi

SET CertPath=%1

signtool sign /tr http://timestamp.sectigo.com /td sha256 /fd sha256 /f %CertPath%  %InstallerDir%\*.msi