@echo off
REM Minesweeper Build Script for Windows
REM Usage: build.bat [clean] [release|debug]

setlocal

set BUILD_TYPE=Release
set CLEAN=0

:parse_args
if "%~1"=="" goto done_args
if /i "%~1"=="clean" set CLEAN=1
if /i "%~1"=="release" set BUILD_TYPE=Release
if /i "%~1"=="debug" set BUILD_TYPE=Debug
shift
goto parse_args

:done_args

if %CLEAN%==1 (
    echo Cleaning build directory...
    if exist build rmdir /s /q build
)

if not exist build mkdir build
cd build

echo Configuring with CMake (%BUILD_TYPE%)...
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if errorlevel 1 (
    echo CMake configuration failed!
    exit /b 1
)

echo Building...
cmake --build . --config %BUILD_TYPE% -j

if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo.
echo Build complete! Executable: build\%BUILD_TYPE%\Minesweeper.exe
echo.
echo To run: cd build\%BUILD_TYPE% ^&^& Minesweeper.exe
echo.

endlocal
