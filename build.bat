@echo off
setlocal
cd /D "%~dp0"

for %%a in (%*) do set "%%a=1"
if not "%release%"=="1" set debug=1
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0 && echo [release mode]

set cl_defines= /DHELIOS_DEBUG=1 /D_HAS_EXCEPTIONS=0 /D_CRT_SECURE_NO_WARNINGS=1
rem /WX
set cl_common=/I..\code\ /MTd /nologo /FC /Z7 /WX /GR-
set cl_opts= %cl_common% /Oi %cl_defines% 
set cl_link_opts=/INCREMENTAL:NO /opt:ref /DYNAMICBASE:NO

set HeliosDLLExports=/EXPORT:GameUpdateAndRender
set HeliosDLLOutDirs= /Fm"build/" /Fe"build/" /Fo"build/" 
set HeliosEXEOutDirs= /Fmbuild/win32_helios.map /Fe"build/helios.exe" /Fo"build/"

if not exist build mkdir build
rem del /f /q "build\*.pdb" 2>nul

rem cl %cl_opts% /P /Fi"build/" code/helios.cpp
cl %cl_opts% %HeliosDLLOutDirs% code/helios.cpp /LD /link /NOENTRY /PDB:"build/helios_dll.pdb" %cl_link_opts% %HeliosDLLExports% ucrt.lib  

rem cl %cl_opts% /P /Fi"build/" code/win32_helios.cpp
cl %cl_opts% %HeliosEXEOutDirs% code/win32_helios.cpp  /link /PDB:"build/win32_helios.pdb" %cl_link_opts% user32.lib gdi32.lib 

for %%a in (%*) do set "%%a=0"
set cl_common=
set cl_opts=
set cl_link_opts=