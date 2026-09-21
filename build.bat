@echo off
setlocal
cd /D "%~dp0"

for %%a in (%*) do set "%%a=1"
if not "%release%"=="1" set debug=1
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0 && echo [release mode]

set cl_defines= /DHELIOS_DEBUG=1 /D_HAS_EXCEPTIONS=0
rem /WX
set cl_common=/I..\code\ /MTd /nologo /FC /Z7 /WX /GR-
set cl_opts= %cl_common% /Oi %cl_defines% 
set cl_link_opts= /link /INCREMENTAL:NO /opt:ref /pdbaltpath:%%%%_PDB%%%% 
if not exist build mkdir build
del /f /q "build\*.pdb" 2>nul

cl %cl_opts% /P /Fi"build/" code/win32_helios.cpp   
cl %cl_opts% /Fmbuild/win32_helios.map /Fe"build/helios.exe" /Fo"build/" code/win32_helios.cpp %cl_link_opts% user32.lib gdi32.lib

rem cl %cl_opts% /P /Fi"build/" code/helios.cpp   
rem cl %cl_opts% /Fe"build/helios.dll" /Fo"build/" code/helios.cpp  /DLL

for %%a in (%*) do set "%%a=0"
set cl_common=
set cl_opts=
set cl_link_opts=