cmake_minimum_required(VERSION 3.2)

#get boost
include( ${WORKSPACE_ROOT_PATH}/thirdparty/boost/boost_include.cmake )

#get gtest
include( ${WORKSPACE_ROOT_PATH}/thirdparty/gtest/gtest_include.cmake )

set( COMMON_TEST_DIR ${COMMON_ROOT_DIR}/tests )

set( COMMON_UNIT_TESTS
	 ${COMMON_TEST_DIR}/angle_tests.cpp
	 ${COMMON_TEST_DIR}/file_tests.cpp
	 ${COMMON_TEST_DIR}/grammar_tests.cpp
	 ${COMMON_TEST_DIR}/inplace_allocator_tests.cpp
	 ${COMMON_TEST_DIR}/math_tests.cpp
	 ${COMMON_TEST_DIR}/preprocessor_tests.cpp
	 ${COMMON_TEST_DIR}/rounding_tests.cpp
	 ${COMMON_TEST_DIR}/tuple_tests.cpp
	 ${COMMON_TEST_DIR}/scheduler_tests.cpp
	 ${COMMON_TEST_DIR}/escape_tests.cpp
	 ${COMMON_TEST_DIR}/stash_tests.cpp )

enable_testing()

add_executable( common_tests ${COMMON_UNIT_TESTS} ${COMMON_TEST_DIR}/main_test_runner.cpp )

set_target_properties( common_tests PROPERTIES FOLDER common )

link_boost( common_tests filesystem )
link_boost( common_tests system )
link_boost( common_tests date_time )
link_boost( common_tests program_options )
link_boost( common_tests iostreams )
link_boost( common_tests atomic )
link_gtest( common_tests )

target_link_libraries(common_tests commonlib)

gtest_add_tests( TARGET common_tests)

install( TARGETS common_tests DESTINATION bin)

add_custom_command( TARGET common_tests POST_BUILD COMMAND ctest --output-on-failure )

