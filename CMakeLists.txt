cmake_minimum_required(VERSION 3.17)

project(
    bencode
    VERSION 1.0
    DESCRIPTION "A simple library for decoding bencoded data"
    LANGUAGES CXX
)
  
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CXX_STANDARD 20)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    
    include(CTest)
endif()

option(BUILD_BENCHMARKS "Build benchmarks" OFF)
option(BUILD_TESTS "Build tests" OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/result/include)

add_subdirectory(source)
add_subdirectory(3rdparty/result)

if (BUILD_TESTS OR BUILD_BENCHMARKS)
    add_subdirectory(helpers)
    include_directories(${CMAKE_SOURCE_DIR}/helpers/include)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if (BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()
 