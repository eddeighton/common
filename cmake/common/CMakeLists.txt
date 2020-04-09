cmake_minimum_required(VERSION 2.8)

#get boost
include( ${COMMON_ROOT_DIR}/cmake/boost_include.cmake )

#get the headers
set( COMMON_API_DIR ${COMMON_ROOT_DIR}/api/common )
file( GLOB COMMON_INCLUDE_HEADERS ${COMMON_API_DIR}/*.hpp ${COMMON_API_DIR}/*.h )

#get the sources
set( COMMON_SOURCE_DIR ${COMMON_ROOT_DIR}/src )
file( GLOB COMMON_SOURCE_HEADERS ${COMMON_SOURCE_DIR}/common/*.hpp )
file( GLOB COMMON_SOURCE_SOURCES ${COMMON_SOURCE_DIR}/common/*.cpp )
set( COMMON_SOURCES ${COMMON_INCLUDE_HEADERS} ${COMMON_SOURCE_HEADERS} ${COMMON_SOURCE_SOURCES} )

source_group( src FILES ${COMMON_SOURCES} )

add_library( commonlib STATIC 
		${COMMON_SOURCES}
		)
		
target_include_directories( commonlib PUBLIC ${COMMON_ROOT_DIR}/api )
		
set_target_properties( commonlib PROPERTIES DEBUG_POSTFIX d )
set_target_properties( commonlib PROPERTIES FOLDER common )

link_boost( commonlib filesystem )
link_boost( commonlib system )

set( InstalledHeaders ${COMMON_INCLUDE_HEADERS} )

install( FILES ${InstalledHeaders} DESTINATION include/common )
install( TARGETS commonlib EXPORT common DESTINATION bin)
install( EXPORT common NAMESPACE Common:: FILE common-config.cmake DESTINATION share )