@echo off

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VS_PATH="

if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_PATH=%%i"
    )
)

if not defined VS_PATH (
    echo ERROR: Could not locate Visual Studio with C++ tools via vswhere.
    exit /b 1
)

call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

call "C:\Program Files (x86)\Intel\oneAPI\setvars.bat" >nul 2>&1
"C:\Program Files\Meson\meson.exe" %*
exit /b %ERRORLEVEL%
