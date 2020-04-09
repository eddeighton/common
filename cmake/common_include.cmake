cmake_minimum_required(VERSION 2.8)

#we assume workspace directory is set

#add_definitions(-D_USE_MATH_DEFINES -D_USE_MATH_DEFINES)
#
#set( COMMON_DIR ${MODULE_DIR}/common )
#
#set( COMMON_INCLUDE_DIR ${COMMON_DIR}/api/common )
#file( GLOB COMMON_INCLUDE_HEADERS ${COMMON_INCLUDE_DIR}/*.hpp ${COMMON_INCLUDE_DIR}/*.h )
#source_group( common FILES ${COMMON_INCLUDE_HEADERS} )
#
#include_directories( ${COMMON_DIR}/api )

function( link_common targetname )

	#target_include_directories( 


	target_link_libraries( ${targetname} commonlib )
endfunction( link_common )
