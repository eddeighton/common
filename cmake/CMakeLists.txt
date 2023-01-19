cmake_minimum_required(VERSION 3.2)

# enable configuration MSVC runtime with CMAKE_MSVC_RUNTIME_LIBRARY
cmake_policy(SET CMP0091 NEW)

project( common )

#enable ctest
enable_testing()

#insist on ALL projects using c++ 17
set( CMAKE_CXX_STANDARD 20 )

######################################
#win32 specific settings
IF( ${WIN32} )

add_definitions(-D_WIN32_WINNT=0x0601)
add_definitions(-DNOMINMAX -D_NOMINMAX)

# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:DebugDLL>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

#enable folders within generated IDE build systems
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

ENDIF( ${WIN32} )

#setup some common directories from the root of the project
set( CMAKE_PARENT_PATH ${CMAKE_CURRENT_LIST_DIR}/.. )
find_path( COMMON_ROOT_DIR NAMES api PATHS ${CMAKE_PARENT_PATH} REQUIRED )
set( COMMON_API_DIR ${COMMON_ROOT_DIR}/api )
set( COMMON_SRC_DIR ${COMMON_ROOT_DIR}/src )

#ensure language runtimes and other system libs are copied to installation folder
include (InstallRequiredSystemLibraries)

add_subdirectory( common )
add_subdirectory( unit_test_runner )
