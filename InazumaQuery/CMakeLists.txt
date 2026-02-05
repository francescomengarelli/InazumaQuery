cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE SOURCES src/*.c)

if(INA_BUILD_SHARED)
    add_library(InazumaQuery SHARED ${SOURCES})
else()
    add_library(InazumaQuery STATIC ${SOURCES})
endif()

target_compile_definitions(InazumaQuery PRIVATE INA_EXPORTS)
target_include_directories(InazumaQuery
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(InazumaQuery
    PUBLIC
        $<$<CONFIG:Debug>:INA_DEBUG_DATA_DIR=\"${INA_DEBUG_DATA_DIR}\">
        $<$<CONFIG:Debug>:INA_DEBUG>
    PRIVATE 
        INA_EXPORTS
)