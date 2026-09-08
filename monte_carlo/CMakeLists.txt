cmake_minimum_required(VERSION 3.10)
project(MonteCarloOptionPricing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(monte_carlo monte_carlo.cpp)

add_executable(monte_carlo_tests
    test_monte_carlo.cpp
    ../third_party/catch2/catch_amalgamated.cpp
)

target_include_directories(monte_carlo_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/catch2
)
