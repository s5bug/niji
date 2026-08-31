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

set "VCVARS_DIR=%VS_PATH%\VC\Auxiliary\Build"
if not exist "%VCVARS_DIR%\vcvars64.bat" (
    echo ERROR: vcvars64.bat not found.
    exit /b 1
)
pushd "%VCVARS_DIR%"
call vcvars64.bat >nul 2>&1
popd

set "SETVARS_DIR=%ProgramFiles(x86)%\Intel\oneAPI"
if not exist "%SETVARS_DIR%\setvars.bat" (
    echo ERROR: Intel oneAPI setvars.bat not found.
    exit /b 1
)
pushd "%SETVARS_DIR%"
call setvars.bat >nul 2>&1
popd

set "MESON_DIR=%ProgramFiles%\Meson"
if not exist "%MESON_DIR%\meson.exe" (
    echo ERROR: meson.exe not found.
    exit /b 1
)
"%MESON_DIR%\meson.exe" %*

exit /b %ERRORLEVEL%

