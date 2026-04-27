@echo off
rem Usage: build_run.bat [debug|release]   (default: debug)
rem Builds via the matching CMake preset, then runs the configured desktop executable.

setlocal

set "ROOT=%~dp0.."
set "CONFIG=%ROOT%\template.ini"
set "project_name=mobile-game-template"
if exist "%CONFIG%" call :load_config "%CONFIG%"

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
cd /d "%ROOT%"

rem Close a previous desktop run so the linker can overwrite the executable.
taskkill /IM "%project_name%.exe" /F >nul 2>nul

if not exist "build\%CFG%\CMakeCache.txt" (
    cmake --preset %CFG% || exit /b 1
)

rem ---- 3. Build ----
cmake --build --preset %CFG% || exit /b 1

rem ---- 4. Run ----
set "EXE=build\%CFG%\%project_name%.exe"
if not exist "%EXE%" (
    if /I "%CFG%"=="debug" set "EXE=build\%CFG%\Debug\%project_name%.exe"
    if /I "%CFG%"=="release" set "EXE=build\%CFG%\Release\%project_name%.exe"
)
if not exist "%EXE%" (
    echo [build_run] %EXE% not found after build. 1>&2
    exit /b 1
)

echo.
echo === Running %EXE% ===
"%EXE%"
exit /b %ERRORLEVEL%

:load_config
for /f "usebackq eol=# tokens=1,* delims==" %%A in ("%~1") do call :set_config "%%A" "%%B"
exit /b 0

:set_config
set "KEY=%~1"
set "VALUE=%~2"
if "%KEY%"=="" exit /b 0
if "%KEY:~0,1%"=="[" exit /b 0
if "%KEY:~0,1%"==";" exit /b 0
if /I "%KEY%"=="project_name" set "project_name=%VALUE%"
exit /b 0
