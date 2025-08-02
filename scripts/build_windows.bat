@echo off
REM Talos Desk - Windows Build Script
REM Requires: Visual Studio 2022, CMake 3.20+, Git

setlocal enabledelayedexpansion

echo ========================================
echo Talos Desk - Windows Build Script
echo ========================================
echo.

REM Set build configuration
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

REM Set architecture
set ARCH=%2
if "%ARCH%"=="" set ARCH=x64

REM Set project root
set PROJECT_ROOT=%~dp0..
cd /d "%PROJECT_ROOT%"

REM Create build directory
set BUILD_DIR=build\windows-%ARCH%-%BUILD_TYPE%
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo Configuration:
echo   Build Type: %BUILD_TYPE%
echo   Architecture: %ARCH%
echo   Build Directory: %BUILD_DIR%
echo.

REM Find Visual Studio installation
where /q cl.exe
if errorlevel 1 (
    echo Error: Visual Studio C++ compiler not found in PATH
    echo Please run this script from a Visual Studio Developer Command Prompt
    exit /b 1
)

REM Check for CMake
where /q cmake.exe
if errorlevel 1 (
    echo Error: CMake not found in PATH
    echo Please install CMake 3.20 or later
    exit /b 1
)

REM Configure with CMake
echo Configuring project with CMake...
cd "%BUILD_DIR%"
cmake -G "Visual Studio 17 2022" -A %ARCH% ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DBUILD_TESTS=ON ^
    -DUSE_HARDWARE_ACCEL=ON ^
    -DENABLE_ONVIF=ON ^
    ../..

if errorlevel 1 (
    echo Error: CMake configuration failed
    exit /b 1
)

REM Build the project
echo.
echo Building project...
cmake --build . --config %BUILD_TYPE% --parallel

if errorlevel 1 (
    echo Error: Build failed
    exit /b 1
)

REM Run tests if built
if exist "bin\%BUILD_TYPE%\talos_desk_tests.exe" (
    echo.
    echo Running tests...
    "bin\%BUILD_TYPE%\talos_desk_tests.exe"
    if errorlevel 1 (
        echo Warning: Some tests failed
    )
)

echo.
echo ========================================
echo Build completed successfully!
echo Executable: %BUILD_DIR%\bin\%BUILD_TYPE%\talos_desk.exe
echo ========================================

REM Copy required DLLs to output directory (if any)
REM TODO: Copy FFmpeg DLLs, etc.

endlocal
exit /b 0