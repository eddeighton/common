
cmake_minimum_required( VERSION 3.1...3.16 )

project( common )

#insist on ALL projects using c++ 17
set( CMAKE_CXX_STANDARD 17 )

######################################
#win32 specific settings
IF( ${WIN32} )

add_definitions(-D_WIN32_WINNT=0x0601)
add_definitions(-DNOMINMAX -D_NOMINMAX)

ENDIF( ${WIN32} )

#setup some common directories from the root of the project
set( CMAKE_PARENT_PATH ${CMAKE_CURRENT_LIST_DIR}/.. )
find_path( COMMON_ROOT_DIR NAMES api PATHS ${CMAKE_PARENT_PATH} )
set( COMMON_API_DIR ${COMMON_ROOT_DIR}/api )
set( COMMON_SRC_DIR ${COMMON_ROOT_DIR}/src )

#helper path if third party libs are in parent folder

find_path( EG_THIRD_PARTY_DIR NAMES boost PATHS ${COMMON_ROOT_DIR}/../../thirdparty_x64 )

#include_directories( ${COMMON_API_DIR} )
#include_directories( ${COMMON_SRC_DIR} )

#attempt to ensure only Debug and Release configs are generated by default
set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo CACHE STRING INTERNAL FORCE)

#enable folders within generated IDE build systems
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#ensure language runtimes and other system libs are copied to installation folder
include (InstallRequiredSystemLibraries)

add_subdirectory( common )
add_subdirectory( unit_test_runner )