@echo off
setlocal

REM Build the Android APK, install it to the connected device, launch it,
REM and stream raylib logs until Ctrl+C.
REM Run from project root: .\scripts\android.bat

set MAKE=C:\msys64\mingw64\bin\mingw32-make.exe
set ADB=C:\android-sdk\platform-tools\adb.exe
set AAPT=C:\android-sdk\build-tools\34.0.0\aapt.exe
set ROOT=%~dp0..
set CONFIG=%ROOT%\template.ini

if not exist "%CONFIG%" (
    echo [android] template.ini not found at "%CONFIG%" 1>&2
    exit /b 1
)

call :load_config "%CONFIG%"

if not defined project_name set "project_name=mobile-game-template"
if not defined display_name set "display_name=Mobile Game Template"
if not defined package_name set "package_name=com.grayfi5hstudio.mobilegametemplate"
if not defined version_code set "version_code=1"
if not defined version_name set "version_name=1.0"
if not defined orientation set "orientation=portrait"
if not defined api_version set "api_version=29"
if not defined apk_name set "apk_name=%project_name%.apk"
if not defined output_dir set "output_dir=dist"
if not defined icon_ldpi set "icon_ldpi=C:\Users\User\raylib\logo\raylib_36x36.png"
if not defined icon_mdpi set "icon_mdpi=C:\Users\User\raylib\logo\raylib_48x48.png"
if not defined icon_hdpi set "icon_hdpi=C:\Users\User\raylib\logo\raylib_72x72.png"

if /I not "%orientation%"=="portrait" if /I not "%orientation%"=="landscape" (
    echo [android] invalid orientation "%orientation%". Use portrait or landscape in template.ini. 1>&2
    exit /b 2
)

if /I "%orientation%"=="portrait" (
    set "orientation_value=1"
) else (
    set "orientation_value=2"
)

set "package_path=%package_name:.=\%"
set "apk_output_dir=..\%output_dir%"
set "apk_path=%ROOT%\%output_dir%\%apk_name%"

echo [android] Project: %project_name%
echo [android] App: %display_name%
echo [android] Package: %package_name%
echo [android] Orientation: %orientation%
echo [android] APK: %apk_path%

cd /d "%ROOT%\src"

echo.
echo [1/5] Cleaning previous build
if exist "android.%project_name%" rmdir /s /q "android.%project_name%"
if exist "%apk_path%" del /q "%apk_path%"
if exist "%apk_path%.idsig" del /q "%apk_path%.idsig"

echo.
echo [2/5] Building APK
"%MAKE%" -f Makefile.Android ^
    ANDROID_API_VERSION=%api_version% ^
    PROJECT_NAME=%project_name% ^
    APK_NAME=%apk_name% ^
    APK_OUTPUT_DIR="%apk_output_dir%" ^
    APP_LABEL_NAME="%display_name%" ^
    APP_PACKAGE_NAME=%package_name% ^
    APP_PACKAGE_PATH=%package_path% ^
    APP_VERSION_CODE=%version_code% ^
    APP_VERSION_NAME=%version_name% ^
    APP_SCREEN_ORIENTATION=%orientation% ^
    APP_ORIENTATION=%orientation_value% ^
    APP_ICON_LDPI="%icon_ldpi%" ^
    APP_ICON_MDPI="%icon_mdpi%" ^
    APP_ICON_HDPI="%icon_hdpi%"
if errorlevel 1 goto fail

echo.
echo [3/5] Verifying APK contents
"%AAPT%" list "%apk_path%" | findstr /c:"classes.dex" >nul
if errorlevel 1 (
    echo ERROR: classes.dex missing from APK
    goto fail
)
"%AAPT%" list "%apk_path%" | findstr /c:"libmain.so" >nul
if errorlevel 1 (
    echo ERROR: libmain.so missing from APK
    goto fail
)
echo OK: classes.dex and libmain.so present

echo.
echo [4/5] Installing to device
"%ADB%" install -r "%apk_path%"
if errorlevel 1 (
    echo Install failed. Uninstalling previous package and retrying...
    "%ADB%" uninstall "%package_name%"
    "%ADB%" install "%apk_path%"
    if errorlevel 1 goto fail
)

echo.
echo [5/5] Launching app and streaming logcat (Ctrl+C to stop)
"%ADB%" shell monkey -p "%package_name%" -c android.intent.category.LAUNCHER 1 >nul
"%ADB%" logcat -c
"%ADB%" logcat raylib:V *:S
exit /b 0

:fail
echo.
echo === Build or install failed ===
exit /b 1

:load_config
for /f "usebackq eol=# tokens=1,* delims==" %%A in ("%~1") do call :set_config "%%A" "%%B"
exit /b 0

:set_config
set "KEY=%~1"
set "VALUE=%~2"
if "%KEY%"=="" exit /b 0
if "%KEY:~0,1%"=="[" exit /b 0
if "%KEY:~0,1%"==";" exit /b 0
set "%KEY%=%VALUE%"
exit /b 0
