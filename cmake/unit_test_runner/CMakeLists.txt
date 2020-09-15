cmake_minimum_required(VERSION 2.8)

#get boost
include( ${COMMON_ROOT_DIR}/cmake/boost_include.cmake )

#get gtest
include( ${COMMON_ROOT_DIR}/cmake/gtest_include.cmake )

#get the unit test utils
#include( ${COMMON_ROOT_DIR}/cmake/test_utils_include.cmake )

#get common
include( ${COMMON_ROOT_DIR}/cmake/common_include.cmake )

set( COMMON_TEST_DIR ${COMMON_ROOT_DIR}/tests )

set( COMMON_UNIT_TESTS
	 ${COMMON_TEST_DIR}/angle_tests.cpp
	 ${COMMON_TEST_DIR}/file_tests.cpp
	 ${COMMON_TEST_DIR}/grammar_tests.cpp
	 ${COMMON_TEST_DIR}/inplace_allocator_tests.cpp
	 ${COMMON_TEST_DIR}/math_tests.cpp
	 ${COMMON_TEST_DIR}/preprocessor_tests.cpp
	 ${COMMON_TEST_DIR}/rounding_tests.cpp
	 ${COMMON_TEST_DIR}/tuple_tests.cpp )

add_executable( common_tests ${COMMON_UNIT_TESTS} ${COMMON_TEST_DIR}/main_test_runner.cpp )

##configure all debug builds to have 'd' postfix
set_target_properties( common_tests PROPERTIES DEBUG_POSTFIX d )
set_target_properties( common_tests PROPERTIES FOLDER common )

link_boost( common_tests filesystem )
link_boost( common_tests system )
link_boost( common_tests date_time )
link_boost( common_tests program_options )
link_boost( common_tests iostreams )
link_gtest( common_tests )
link_common( common_tests )

install( TARGETS common_tests DESTINATION bin)
