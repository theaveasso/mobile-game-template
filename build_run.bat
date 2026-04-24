@echo off
rem Usage: build_run.bat [debug|release]   (default: debug)
rem Builds via the matching CMake preset, then runs build\<cfg>\game01.exe.

setlocal

set "CFG=%~1"
if "%CFG%"=="" set "CFG=debug"
if /I not "%CFG%"=="debug" if /I not "%CFG%"=="release" (
    echo [build_run] usage: %~n0 [debug^|release] 1>&2
    exit /b 2
)

rem ---- 1. Initialize MSVC environment (once per shell) ----
if defined VSCMD_ARG_TGT_ARCH goto :after_env

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [build_run] vswhere.exe not found. Is Visual Studio installed? 1>&2
    exit /b 1
)

for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%i"
if not defined VSINSTALL (
    echo [build_run] MSVC x64 toolset not found in any VS install. 1>&2
    exit /b 1
)

call "%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1

:after_env

rem ---- 2. Configure (only if cache is missing) ----
if not exist "build\%CFG%\CMakeCache.txt" (
    cmake --preset %CFG% || exit /b 1
)

rem ---- 3. Build ----
cmake --build --preset %CFG% || exit /b 1

rem ---- 4. Run ----
set "EXE=build\%CFG%\game01.exe"
if not exist "%EXE%" (
    echo [build_run] %EXE% not found after build. 1>&2
    exit /b 1
)

echo.
echo === Running %EXE% ===
"%EXE%"
exit /b %ERRORLEVEL%
