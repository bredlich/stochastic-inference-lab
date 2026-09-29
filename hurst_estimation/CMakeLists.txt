cmake_minimum_required(VERSION 3.10)
project(HurstExponentEstimation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT DEFINED EIGEN_INCLUDE_DIR)
    set(EIGEN_INCLUDE_DIR "C:/Users/bjr30/Libraries/eigen-5.0.0")
endif()

add_executable(hurst hurst.cpp)
target_include_directories(hurst PRIVATE ${EIGEN_INCLUDE_DIR})

add_executable(hurst_tests
    test_hurst.cpp
    ../third_party/catch2/catch_amalgamated.cpp
)
target_include_directories(hurst_tests PRIVATE
    ${EIGEN_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/catch2
)

# libpqxx: vcpkg (Windows) provides CMake config files; apt (Linux CI)
# provides pkg-config files instead. Try CONFIG mode first, fall back
# to pkg-config so the same CMakeLists.txt works on both.
find_package(libpqxx CONFIG QUIET)

add_executable(store_hurst_results store_hurst_results.cpp)

if(libpqxx_FOUND)
    target_link_libraries(store_hurst_results PRIVATE libpqxx::pqxx)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PQXX REQUIRED libpqxx)
    target_include_directories(store_hurst_results PRIVATE ${PQXX_INCLUDE_DIRS})
    target_link_libraries(store_hurst_results PRIVATE ${PQXX_LIBRARIES})
    target_link_directories(store_hurst_results PRIVATE ${PQXX_LIBRARY_DIRS})
endif()
