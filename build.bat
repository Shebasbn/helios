@echo off
rem call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
cls

set dir_path=%CD%
set INCLUDES=/I%dir_path%\src\
set CODE_EXE=%dir_path%\src\helios_main.cpp
set CODE_DLL=%dir_path%\src\game\helios.cpp
set CODE_P=%dir_path%\build\helios.i
set CODE_PDLL=%dir_path%\build\helios_dll.i
set EXE_NAME=helios.exe
set DLL_NAME=helios_dll.dll
set LIBS=user32.lib gdi32.lib

rem I/S == Internal/Release - S/F: Slow/Fast
set IS_CL_OPT=-DHELIOS_INTERNAL -DHELIOS_SLOW /Od /Zi /RTC1 /MDd /JMC %CL_OPT%
set IS_LINK_FLAGS= /DEBUG


set IF_CL_OPT=-DHELIOS_INTERNAL /O2 /Oi /Zi /Zo /MD %CL_OPT%
set IF_LINK_FLAGS=/DEBUG:FULL

set RS_CL_OPT=-DHELIOS_SLOW /O2 /Oi /Zi /Zo /MD %CL_OPT%
set RS_LINK_FLAGS=/DEBUG:FULL

set RF_CL_OPT=/O2 /Oi /Gy /GL /MD /DNDEBUG %CL_OPT%
set RF_LINK_FLAGS=/OPT:REF /OPT:ICF

set CL_OPT=/nologo /DUNICODE /D_UNICODE /FC /W4 %IS_CL_OPT% %INCLUDES%
set LINK_FLAGS=/INCREMENTAL:NO %IS_LINK_FLAGS%

IF NOT EXIST build mkdir build
pushd build
call cl %CL_OPT% %CODE_DLL% /Fe%DLL_NAME% /LD /link %LINK_FLAGS% 
call cl %CL_OPT% %CODE_EXE% /Fe%EXE_NAME% /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup %LINK_FLAGS% %LIBS%


call cl /EP /P %CL_OPT% /Fihelios.i %CODE_EXE% 
call cl /EP /P %CL_OPT% /Fihelios_dll.i %CODE_DLL% 
call cl %CL_OPT% /Tp %CODE_PDLL% /Fehelios_debug.dll /LD /link %LINK_FLAGS%
call cl %CL_OPT% /Tp %CODE_P% /Fehelios_debug.exe /link %LINK_FLAGS% %LIBS%
popd