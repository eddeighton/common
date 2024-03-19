cmake_minimum_required(VERSION 3.2)

project( common )

include( ${WORKSPACE_ROOT_PATH}/src/env/src/common_settings.cmake )

#setup some common directories from the root of the project
set( CMAKE_PARENT_PATH ${CMAKE_CURRENT_LIST_DIR}/.. )
find_path( COMMON_ROOT_DIR NAMES api PATHS ${CMAKE_PARENT_PATH} REQUIRED )
set( COMMON_API_DIR ${COMMON_ROOT_DIR}/api )
set( COMMON_SRC_DIR ${COMMON_ROOT_DIR}/src )

add_subdirectory( common )
add_subdirectory( unit_test_runner )
