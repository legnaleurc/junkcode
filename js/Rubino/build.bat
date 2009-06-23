@echo off
setlocal

set OUTZIP=build\Rubino.xpi

set EXE="C:\Program Files\7-Zip\7z.exe"
set ARGS=a -tzip -r -mx9

%EXE% %ARGS% %OUTZIP% .\src\* -xr!.svn\ -xr!*~
