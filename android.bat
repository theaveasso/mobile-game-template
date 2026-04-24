@echo off
setlocal

REM Build the Android APK, install it to the connected device, launch it,
REM and stream raylib logs until Ctrl+C.
REM Run from project root: .\android.bat

set MAKE=C:\msys64\mingw64\bin\mingw32-make.exe
set ADB=C:\android-sdk\platform-tools\adb.exe
set AAPT=C:\android-sdk\build-tools\34.0.0\aapt.exe
set PKG=com.grayfi5hstudio.game01
set APK=game01.apk

cd /d %~dp0\src

echo.
echo [1/5] Cleaning previous build
if exist android.game01 rmdir /s /q android.game01
if exist %APK% del /q %APK%

echo.
echo [2/5] Building APK
%MAKE% -f Makefile.Android ANDROID_API_VERSION=29
if errorlevel 1 goto fail

echo.
echo [3/5] Verifying APK contents
%AAPT% list %APK% | findstr /c:"classes.dex" >nul
if errorlevel 1 (
    echo ERROR: classes.dex missing from APK
    goto fail
)
%AAPT% list %APK% | findstr /c:"libmain.so" >nul
if errorlevel 1 (
    echo ERROR: libmain.so missing from APK
    goto fail
)
echo OK: classes.dex and libmain.so present

echo.
echo [4/5] Installing to device
%ADB% install -r %APK%
if errorlevel 1 (
    echo Install failed. Uninstalling previous package and retrying...
    %ADB% uninstall %PKG%
    %ADB% install %APK%
    if errorlevel 1 goto fail
)

echo.
echo [5/5] Launching app and streaming logcat (Ctrl+C to stop)
%ADB% shell monkey -p %PKG% -c android.intent.category.LAUNCHER 1 >nul
%ADB% logcat -c
%ADB% logcat raylib:V *:S
exit /b 0

:fail
echo.
echo === Build or install failed ===
exit /b 1
