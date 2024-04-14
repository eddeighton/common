cmake_minimum_required(VERSION 3.2)

project( common )

include( ${WORKSPACE_ROOT_PATH}/src/env/src/common_settings.cmake )

#message( STATUS "Current Path: ${CMAKE_CURRENT_LIST_DIR}" )
#setup some common directories from the root of the project

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH CMAKE_PARENT_PATH )
#set( CMAKE_PARENT_PATH ${CMAKE_CURRENT_LIST_DIR}/.. )
#message( STATUS "CMAKE_PARENT_PATH: ${CMAKE_PARENT_PATH}" )

find_path( COMMON_ROOT_DIR HINTS ${CMAKE_PARENT_PATH} NAMES "api" PATHS CMAKE_PARENT_PATH REQUIRED NO_DEFAULT_PATH ONLY_CMAKE_FIND_ROOT_PATH )
#message( STATUS "COMMON_ROOT_DIR: ${COMMON_ROOT_DIR}" )

set( COMMON_API_DIR ${COMMON_ROOT_DIR}/api )
set( COMMON_SRC_DIR ${COMMON_ROOT_DIR}/src )

add_subdirectory( common )
add_subdirectory( inja )
add_subdirectory( unit_test_runner )
