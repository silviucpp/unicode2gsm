cmake_minimum_required(VERSION 3.10)
project(unicode2gsm)

set(CMAKE_CXX_STANDARD 11)

if(DEFINED ENV{ENABLE_UNIT_TEST})
    add_subdirectory(tests)  
endif()

include_directories(include)

file(GLOB_RECURSE sources src/*.cc include/*.h)
string (REPLACE ";" "\n    " FILES_MSG "${sources}")
message(STATUS "source files:\n    ${FILES_MSG}")

add_library(unicode2gsm ${sources})
