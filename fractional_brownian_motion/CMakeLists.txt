cmake_minimum_required(VERSION 3.10)
project(FractionalBrownianMotion)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT DEFINED EIGEN_INCLUDE_DIR)
    set(EIGEN_INCLUDE_DIR "C:/Users/bjr30/Libraries/eigen-5.0.0")
endif()

add_executable(fbm fbm.cpp)
target_include_directories(fbm PRIVATE ${EIGEN_INCLUDE_DIR})
