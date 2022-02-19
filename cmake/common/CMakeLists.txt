cmake_minimum_required(VERSION 3.2)

#get boost
include( ${WORKSPACE}/root/thirdparty/boost/boost_include.cmake )

#get the headers
set( COMMON_API_DIR ${COMMON_ROOT_DIR}/api/common )
file( GLOB COMMON_INCLUDE_HEADERS ${COMMON_API_DIR}/*.hpp ${COMMON_API_DIR}/*.h )

#get the sources
set( COMMON_SOURCE_DIR ${COMMON_ROOT_DIR}/src )
file( GLOB COMMON_SOURCE_HEADERS ${COMMON_SOURCE_DIR}/common/*.hpp )
file( GLOB COMMON_SOURCE_SOURCES ${COMMON_SOURCE_DIR}/common/*.cpp )
set( COMMON_SOURCES ${COMMON_INCLUDE_HEADERS} ${COMMON_SOURCE_HEADERS} ${COMMON_SOURCE_SOURCES} )

source_group( src FILES ${COMMON_SOURCES} )

add_library( commonlib ${CFG_LINK} ${COMMON_SOURCES} )
add_library( Common::commonlib ALIAS commonlib )

target_include_directories( commonlib PUBLIC ${COMMON_ROOT_DIR}/api )

set_target_properties( commonlib PROPERTIES FOLDER common )

link_boost( commonlib filesystem )
link_boost( commonlib system )
link_boost( commonlib timer )
link_boost( commonlib iostreams )

set( InstalledHeaders ${COMMON_INCLUDE_HEADERS} )

install( FILES ${InstalledHeaders} DESTINATION include/common )
install( TARGETS commonlib EXPORT common DESTINATION bin)
install( EXPORT common NAMESPACE Common:: FILE common-config.cmake DESTINATION share )
