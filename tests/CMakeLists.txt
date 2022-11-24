cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)

include_directories(../include/)

file(GLOB_RECURSE TEST_FILES *.cc)
string (REPLACE ";" "\n    " TEST_FILES_MSG "${TEST_FILES}")
message(STATUS "Test files:\n    ${TEST_FILES_MSG}")

add_executable(unicode2gsm_test ${TEST_FILES})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

include_directories(${BOOST_INCLUDEDIR})

target_link_libraries(unicode2gsm_test unicode2gsm ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ${Boost_LIBRARIES})
