@echo off
setlocal

set "CMAKE_C_COMPILER="
set "CMAKE_CXX_COMPILER="

:: 1. Setup MSVC Environment (only if cl.exe isn't already in the PATH)
where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo [Build] Initializing MSVC Environment...
    :: Adjust this path to match your VS installation year/edition (Community/Professional)
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"  amd64 -vcvars_ver=14.3>nul
)

if not exist build mkdir build

:: 3. Configure the project with Ninja (Run once, or automatically if CMakeLists changes)
if not exist build\build.ninja (
    echo [Build] Configuring CMake with Ninja...
    cmake -G "Ninja" -B build -S .
)

:: 4. Run the build
echo [Build] Compiling project...
cmake --build build

endlocal