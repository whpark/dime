@echo off
set project=dime.sln
rem call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
echo ------------------
echo +  x64 - Debug
echo ------------------
msbuild %project% /p:Platform=x64 /p:Configuration=Debug -m
if %ERRORLEVEL% neq 0  goto :error_end
echo ------------------
echo +  x64 - Release
echo ------------------
msbuild %project% /p:Platform=x64 /p:Configuration=Release -m
if %ERRORLEVEL% neq 0  goto :error_end
echo ------------------
echo +  win32 - Debug
echo ------------------
msbuild %project% /p:Platform=x86 /p:Configuration=Debug -m
if %ERRORLEVEL% neq 0  goto :error_end
echo ------------------
echo +  win32 - Release
echo ------------------
msbuild %project% /p:Platform=x86 /p:Configuration=Release -m
if %ERRORLEVEL% neq 0  goto :error_end

goto :end

:error_end
@echo ----------- ERROR -----------

:end
pause
