cmake_minimum_required(VERSION 2.8)

#get boost
include( ${COMMON_ROOT_DIR}/cmake/boost_include.cmake )

#get gtest
include( ${THIRD_PARTY_DIR}/gtest/gtest_1_7_0_vs2017_include.cmake )

#get the unit test utils
include( ${MODULE_DIR}/test_utils/cmake/test_utils_include.cmake )

#get common
include( ${COMMON_ROOT_DIR}/cmake/common_include.cmake )

#get the tests
include( ${MODULE_DIR}/common/cmake/common_tests_include.cmake )

add_executable( common_tests
			${COMMON_INCLUDE_HEADERS}
            ${COMMON_TEST_SOURCES}
			${TEST_UTILS_SRC_HEADERS}
			${TEST_UTILS_SRC_SOURCES}
			)

##configure all debug builds to have 'd' postfix
set_target_properties( common_tests PROPERTIES DEBUG_POSTFIX d )
set_target_properties( common_tests PROPERTIES FOLDER common )

link_boost_usual( common_tests )
link_gtest( common_tests )
link_common( common_tests )

install( TARGETS common_tests DESTINATION bin)
