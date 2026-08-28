@echo off
REM =====================================================
REM Pixel Gun 3D Trainer — Launch Script (Windows)
REM Launches PG3D_Injector.exe to inject trainer.dll
REM =====================================================

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "BIN_DIR=%SCRIPT_DIR%"

REM Check if we are running from source tree (scripts\) instead of release package
if not exist "%BIN_DIR%PG3D_Injector.exe" (
    set "PROJECT_ROOT=%SCRIPT_DIR%.."
    set "BUILD_DIR=!PROJECT_ROOT!\build"
    set "BIN_DIR=!BUILD_DIR!\bin\"
)

title PG3D Trainer — Launcher

echo.
echo   ╔══════════════════════════════════════════════╗
echo   ║       PIXEL GUN 3D PC TRAINER                ║
echo   ║       Launcher (Windows)                     ║
echo   ╚══════════════════════════════════════════════╝
echo.

if "%~1"=="" goto Inject
if /i "%~1"=="inject" goto Inject
if /i "%~1"=="status" goto Status
if /i "%~1"=="help" goto Help

echo   [!] Unknown command: %1
echo   Run "%~nx0 help" for usage.
exit /b 1

REM =====================================================
REM Check Build
REM =====================================================
:CheckBuild
REM Auto-copy libtrainer.dll to trainer.dll if needed
if not exist "%BIN_DIR%trainer.dll" (
    if exist "%BIN_DIR%libtrainer.dll" (
        echo   [*] Found libtrainer.dll, copying to trainer.dll for injector compatibility...
        copy /y "%BIN_DIR%libtrainer.dll" "%BIN_DIR%trainer.dll" >nul
    )
)

if not exist "%BIN_DIR%trainer.dll" (
    echo   [!] trainer.dll not found.
    echo       Make sure you have compiled the code or downloaded a release.
    exit /b 1
)

if not exist "%BIN_DIR%PG3D_Injector.exe" (
    echo   [!] PG3D_Injector.exe not found.
    echo       Make sure you have compiled the code or downloaded a release.
    exit /b 1
)

echo   [+] Binaries found
exit /b 0

REM =====================================================
REM Inject
REM =====================================================
:Inject
call :CheckBuild
if %errorlevel% neq 0 exit /b 1

echo.
echo   [!] Make sure Pixel Gun 3D is running via Steam
echo   [!] before the injector starts scanning.
echo.
echo   [*] Launching PG3D_Injector.exe...
echo.

REM Start without waiting for exit
start "" "%BIN_DIR%PG3D_Injector.exe"
exit /b 0

REM =====================================================
REM Status
REM =====================================================
:Status
echo   [*] Binaries:
if exist "%BIN_DIR%trainer.dll" (
    echo   [+]   trainer.dll      OK
) else (
    echo   [!]   trainer.dll      MISSING
)

if exist "%BIN_DIR%PG3D_Injector.exe" (
    echo   [+]   PG3D_Injector    OK
) else (
    echo   [!]   PG3D_Injector    MISSING
)

echo.
echo   [*] Game location:
set "STEAM_COMMON=%ProgramFiles(x86)%\Steam\steamapps\common\Pixel Gun 3D"
if exist "%STEAM_COMMON%" (
    echo   [+]   Found at: %STEAM_COMMON%
) else (
    echo   [!]   Not found in default Steam path.
)
exit /b 0

REM =====================================================
REM Help
REM =====================================================
:Help
echo   Usage: %~nx0 [command]
echo.
echo   Commands:
echo     inject     — Launch injector (default)
echo     status     — Check build artifacts and game location
echo     help       — Show this help message
echo.
echo   Prerequisites:
echo     1. Ensure trainer.dll and PG3D_Injector.exe exist
echo     2. Start Pixel Gun 3D via Steam
echo     3. Run this script to inject
echo.
exit /b 0