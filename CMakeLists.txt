cmake_minimum_required(VERSION 3.15)
project(helios CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/int")

set(CORE_SOURCES 
    "core/common.h"
    "core/core.h"
	"core/arena.h"
	"core/core_string.h"
    "core/platform.h"
	"core/internal.h"	
	"core/core.cpp"
    "core/platform_win32.cpp"
	"core/arena.cpp"
)

add_library(core STATIC ${CORE_SOURCES})
target_include_directories(core PUBLIC "${CMAKE_SOURCE_DIR}/core")

if(WIN32)
    # Windows-specific code or library linking goes here
    target_link_libraries(core PUBLIC ws2_32 gdi32 user32 winmm)
else()
    # Linux, macOS, or other operating systems
    target_link_libraries(core PUBLIC pthread)
endif()


set(GAME_MODULE_SOURCES 
	"helios/sim.h"
	"helios/sim.cpp"
)

add_library(game_module MODULE  ${GAME_MODULE_SOURCES})
target_include_directories(game_module
	PRIVATE
		"${CMAKE_SOURCE_DIR}/helios"
		"${CMAKE_SOURCE_DIR}/core"
)

set(GAME_SOURCES 
	"helios/helios.cpp"
	"helios/helios.h"
)
add_executable(helios ${GAME_SOURCES})
target_include_directories(helios PRIVATE "${CMAKE_SOURCE_DIR}/helios")

target_link_libraries(helios PRIVATE core)

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/bin/data")
    if(CMAKE_HOST_WIN32)
        # Windows developer mode allows symlinks without admin
        execute_process(COMMAND cmd /c mklink /J "${CMAKE_SOURCE_DIR}/bin/data" "${CMAKE_SOURCE_DIR}/data")
    else()
        execute_process(COMMAND ln -s "${CMAKE_SOURCE_DIR}/data" "${CMAKE_SOURCE_DIR}/bin/data")
    endif()
endif()