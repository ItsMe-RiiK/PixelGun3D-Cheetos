@echo off
REM =====================================================
REM Pixel Gun 3D Trainer — Offset Auto-Updater
REM Runs validate_offsets.py to update offsets.h from dump.cs
REM =====================================================

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
set "TOOLS_DIR=%PROJECT_ROOT%\resources\tools"
set "DUMPED_DIR=%PROJECT_ROOT%\resources\dumped"
set "VALIDATOR=%TOOLS_DIR%\validate_offsets.py"
set "LOG_DIR=%PROJECT_ROOT%\resources\logs"

REM Generate timestamp
for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set "DT=%%I"
set "TIMESTAMP=%DT:~0,8%_%DT:~8,6%"
set "LOG_FILE=%LOG_DIR%\validate.log"

title PG3D Offset Updater

echo.
echo   ╔═════════════════════════════════════════════╗
echo   ║       PIXEL GUN 3D — Offset Updater         ║
echo   ╚═════════════════════════════════════════════╝
echo.

if "%~1"=="" goto Validate
if /i "%~1"=="validate" goto Validate
if /i "%~1"=="update" goto Update
if /i "%~1"=="help" goto Help

echo   [!] Unknown command: %1
echo   Run "%~nx0 help" for usage.
exit /b 1

REM =====================================================
REM Pre-flight
REM =====================================================
:Preflight
where python >nul 2>&1
if %errorlevel% neq 0 (
    where python3 >nul 2>&1
    if %errorlevel% neq 0 (
        echo   [!] Python is required but not found.
        echo       Download from https://www.python.org/downloads/
        exit /b 1
    )
    set "PYTHON=python3"
) else (
    set "PYTHON=python"
)

if not exist "%VALIDATOR%" (
    echo   [!] Validator script not found at: %VALIDATOR%
    exit /b 1
)

if not exist "%DUMPED_DIR%\dump.cs" (
    echo   [!] dump.cs not found at: %DUMPED_DIR%\dump.cs
    echo.
    echo   [!] Please place your IL2CPP dump files in:
    echo       %DUMPED_DIR%\
    echo.
    echo   Required files:
    echo     dump.cs          — IL2CPP class dump
    echo     il2cpp.h         — IL2CPP header (optional)
    echo     script.json      — IL2CPP script dump (optional)
    exit /b 1
)

echo   [+] Pre-flight checks passed
exit /b 0

REM =====================================================
REM Validate
REM =====================================================
:Validate
call :Preflight
if %errorlevel% neq 0 exit /b 1

echo   [*] Running offset auto-updater...
echo.

REM Create log directory
if not exist "%LOG_DIR%" mkdir "%LOG_DIR%"

REM Run updater and log
%PYTHON% "%VALIDATOR%" > "%LOG_FILE%" 2>&1
type "%LOG_FILE%"

echo.

REM Check for results
findstr /C:"[UPDATED]" "%LOG_FILE%" >nul 2>&1
if %errorlevel% equ 0 (
    echo   [+] Offsets were successfully updated! Review the output above.
)
findstr /C:"[WARNING]" "%LOG_FILE%" >nul 2>&1
if %errorlevel% equ 0 (
    echo   [!] Some offsets were not found. Review the output above.
    echo.
    echo   [!] You may need to check the obfuscated field names in:
    echo       %PROJECT_ROOT%\src\utils\offsets.h
    echo.
)
findstr /C:"[UPDATED]" "%LOG_FILE%" >nul 2>&1
if %errorlevel% neq 0 (
    findstr /C:"[WARNING]" "%LOG_FILE%" >nul 2>&1
    if %errorlevel% neq 0 (
        echo   [+] All offsets are already up-to-date!
    )
)

echo   [+] Log saved to: %LOG_FILE%
exit /b 0

REM =====================================================
REM Update
REM =====================================================
:Update
call :Preflight
if %errorlevel% neq 0 exit /b 1

if "%~2"=="" (
    echo   Usage: %~nx0 update ^<path-to-new-dumps^>
    echo.
    echo   The directory should contain at least dump.cs
    echo   Optionally: il2cpp.h, script.json, stringliteral.json
    exit /b 1
)

set "NEW_DUMP_DIR=%~2"

if not exist "%NEW_DUMP_DIR%" (
    echo   [!] Directory not found: %NEW_DUMP_DIR%
    exit /b 1
)

if not exist "%NEW_DUMP_DIR%\dump.cs" (
    echo   [!] dump.cs not found in: %NEW_DUMP_DIR%
    exit /b 1
)

REM Backup current dumps
set "BACKUP_DIR=%DUMPED_DIR%\backup_%TIMESTAMP%"
echo   [*] Backing up current dumps to: %BACKUP_DIR%
mkdir "%BACKUP_DIR%"

for %%F in (dump.cs il2cpp.h script.json stringliteral.json) do (
    if exist "%DUMPED_DIR%\%%F" (
        copy /Y "%DUMPED_DIR%\%%F" "%BACKUP_DIR%\%%F" >nul
    )
)
echo   [+] Backup complete

REM Copy new dumps
echo   [*] Copying new dump files...
for %%F in (dump.cs il2cpp.h script.json stringliteral.json) do (
    if exist "%NEW_DUMP_DIR%\%%F" (
        copy /Y "%NEW_DUMP_DIR%\%%F" "%DUMPED_DIR%\%%F" >nul
        echo   [+]   Copied: %%F
    )
)

echo.
echo   [+] Dump files updated!
echo.
echo   [*] Running auto-updater against new dumps...
echo.

goto Validate

REM =====================================================
REM Help
REM =====================================================
:Help
echo   Usage: %~nx0 ^<command^> [args]
echo.
echo   Commands:
echo     validate         — Run auto-updater against dump.cs
echo     update ^<path^>    — Update dump files from a directory and run auto-updater
echo     help             — Show this help message
echo.
echo   Examples:
echo     %~nx0 validate
echo     %~nx0 update C:\Users\You\Downloads\new_dumps
echo.
exit /b 0
