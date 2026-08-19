@echo off
REM =====================================================
REM Pixel Gun 3D Trainer — Build Script (Windows)
REM Compiles trainer.dll ^& PG3D_Injector.exe
REM using CMake + vcpkg
REM =====================================================

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
set "BUILD_DIR=%PROJECT_ROOT%\build"
set "BIN_DIR=%BUILD_DIR%\bin"
set "VCPKG_DIR=%PROJECT_ROOT%\vcpkg"
set "TRIPLET=x64-windows"

title PG3D Trainer — Build

echo.
echo   ╔════════════════════════════════════════════╗
echo   ║       PIXEL GUN 3D PC - Builder            ║
echo   ║       Build Script (Windows)               ║
echo   ╚════════════════════════════════════════════╝
echo.

if "%~1"=="" goto Release
if /i "%~1"=="release" goto Release
if /i "%~1"=="debug" goto Debug
if /i "%~1"=="clean" goto Clean
if /i "%~1"=="rebuild" goto Rebuild
if /i "%~1"=="help" goto Help

echo   [!] Unknown command: %1
echo   Run "%~nx0 help" for usage.
exit /b 1

REM =====================================================
REM Check Dependencies
REM =====================================================
:CheckDeps
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo   [!] CMake not found. Install from https://cmake.org/download/
    exit /b 1
)

where cl >nul 2>&1
if %errorlevel% neq 0 (
    where g++ >nul 2>&1
    if %errorlevel% neq 0 (
        echo   [!] No C++ compiler found.
        echo       Install Visual Studio Build Tools or MinGW-w64.
        echo       Or run from a "Developer Command Prompt for VS".
        exit /b 1
    )
)

echo   [+] Dependencies OK
exit /b 0

REM =====================================================
REM Setup vcpkg
REM =====================================================
:SetupVcpkg
if not exist "%VCPKG_DIR%\vcpkg.exe" (
    echo   [*] Bootstrapping vcpkg...
    if not exist "%VCPKG_DIR%\bootstrap-vcpkg.bat" (
        echo   [!] vcpkg submodule missing. Initializing...
        git -C "%PROJECT_ROOT%" submodule update --init --recursive
    )
    call "%VCPKG_DIR%\bootstrap-vcpkg.bat" -disableMetrics
)
echo   [+] vcpkg ready
exit /b 0

REM =====================================================
REM Build (internal)
REM =====================================================
:DoBuild
call :CheckDeps
if %errorlevel% neq 0 exit /b 1

call :SetupVcpkg
if %errorlevel% neq 0 exit /b 1

echo   [*] Configuring CMake (triplet: %TRIPLET%, type: %BUILD_TYPE%)...
cmake -S "%PROJECT_ROOT%" -B "%BUILD_DIR%" ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake" ^
    -DVCPKG_TARGET_TRIPLET=%TRIPLET% ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if %errorlevel% neq 0 (
    echo   [!] CMake configure failed.
    exit /b 1
)

echo   [*] Building...
cmake --build "%BUILD_DIR%" --config %BUILD_TYPE% -j %NUMBER_OF_PROCESSORS%

if %errorlevel% neq 0 (
    echo   [!] Build failed.
    exit /b 1
)

if not exist "%BIN_DIR%\trainer.dll" (
    echo   [!] trainer.dll not found in build output.
    exit /b 1
)

echo.
echo   [+] Build complete!
echo   [+] trainer.dll     -^> %BIN_DIR%\trainer.dll
echo   [+] PG3D_Injector   -^> %BIN_DIR%\PG3D_Injector.exe
echo.
echo   [*] To launch, run: scripts\runWindows.bat
exit /b 0

REM =====================================================
REM Release
REM =====================================================
:Release
set "BUILD_TYPE=Release"
goto DoBuild

REM =====================================================
REM Debug
REM =====================================================
:Debug
set "BUILD_TYPE=Debug"
goto DoBuild

REM =====================================================
REM Clean
REM =====================================================
:Clean
if exist "%BUILD_DIR%" (
    echo   [*] Removing build directory...
    rmdir /s /q "%BUILD_DIR%"
    echo   [+] Clean complete
) else (
    echo   [*] Nothing to clean
)
exit /b 0

REM =====================================================
REM Rebuild
REM =====================================================
:Rebuild
call :Clean
set "BUILD_TYPE=Release"
goto DoBuild

REM =====================================================
REM Help
REM =====================================================
:Help
echo   Usage: %~nx0 [command]
echo.
echo   Commands:
echo     release    — Build in Release mode (default)
echo     debug      — Build in Debug mode
echo     clean      — Remove build directory
echo     rebuild    — Clean + build Release
echo     help       — Show this help message
echo.
exit /b 0
