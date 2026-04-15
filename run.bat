@echo off
setlocal enabledelayedexpansion

echo [INFO] Lua Script Engine Setup
echo.

:: Check for Git Bash / MSYS2
where bash >nul 2>&1
if %errorlevel%==0 (
    echo [INFO] Found bash, using run.sh...
    bash run.sh %*
    exit /b %errorlevel%
)

:: Check Chocolatey
where choco >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARN] Chocolatey not found. Installing...
    powershell -NoProfile -ExecutionPolicy Bypass -Command ^
        "[System.Net.ServicePointManager]::SecurityProtocol = 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"
    
    if %errorlevel% neq 0 (
        echo [ERROR] Failed to install Chocolatey
        echo Please install manually: https://chocolatey.org/install
        exit /b 1
    )
    
    :: Refresh PATH
    call refreshenv
)

:: Check CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [INFO] Installing cmake...
    choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
    call refreshenv
)

:: Check Lua
where lua >nul 2>&1
if %errorlevel% neq 0 (
    echo [INFO] Installing lua...
    choco install lua -y
    call refreshenv
)

:: Check Visual Studio Build Tools
where cl >nul 2>&1
if %errorlevel% neq 0 (
    echo [INFO] Installing Visual Studio Build Tools...
    choco install visualstudio2022buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools --includeRecommended" -y
    call refreshenv
)

echo [INFO] Building project...
cd backend

if exist build rmdir /s /q build
mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF ..
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed
    exit /b 1
)

cmake --build . --config Release
if %errorlevel% neq 0 (
    echo [ERROR] Build failed
    exit /b 1
)

cd ..\..

echo.
echo [INFO] Starting Lua Script Engine...
echo.

backend\build\Release\lua_engine.exe backend\scripts %*

endlocal
