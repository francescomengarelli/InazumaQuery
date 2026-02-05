cmake_minimum_required(VERSION 3.15)

option(INA_BUILD_SHARED "Build a shared library" ON)
option(INA_BUILD_TESTS "Enable building of tests" OFF)
set(INA_DEBUG_DATA_DIR "${CMAKE_SOURCE_DIR}/data")

if(INA_BUILD_TESTS AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "setting CMAKE_BUILD_TYPE to Debug to allow INA_BUILD_TESTS to work properly")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif(INA_BUILD_TESTS AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")

project(InazumaQuery LANGUAGES C VERSION 0.1)

add_subdirectory(InazumaQuery)

if(INA_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
