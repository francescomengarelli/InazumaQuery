cmake_minimum_required(VERSION 3.15)
project(InazumaQuery_tests LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE        # silence CMP0135 warning
)
FetchContent_MakeAvailable(googletest)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

foreach(TEST_SRC IN LISTS TEST_SOURCES)
  get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)

  add_executable(${TEST_NAME} ${TEST_SRC})
  target_link_libraries(${TEST_NAME}
    PRIVATE InazumaQuery
  )

  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

  set_tests_properties(${TEST_NAME} PROPERTIES
    ENVIRONMENT "DYLD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/InazumaQuery"
  )
endforeach()
