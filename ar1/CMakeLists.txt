cmake_minimum_required(VERSION 3.10)
project(AR1ProcessSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(ar1 ar1.cpp)

add_executable(ar1_tests
    test_ar1.cpp
    ../third_party/catch2/catch_amalgamated.cpp
)
target_include_directories(ar1_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/catch2
)
