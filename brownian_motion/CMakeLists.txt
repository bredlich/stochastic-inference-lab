cmake_minimum_required(VERSION 3.10)
project(BrownianMotion)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# The main program
add_executable(brownian_motion brownian_motion.cpp)

# The test executable - includes Catch2's own source file plus your tests
add_executable(brownian_motion_tests
    test_brownian_motion.cpp
    ../third_party/catch2/catch_amalgamated.cpp
)

# So both targets can find brownian_motion.h and Catch2's header
target_include_directories(brownian_motion_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/catch2
)
