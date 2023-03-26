cmake_minimum_required(VERSION 3.24.2)
project(CoreGame VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

find_program(CCACHE_EXECUTABLE ccache)
if(CCACHE_EXECUTABLE)
  message(STATUS "ccache found")
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE}) 
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE})
else()
  message(STATUS "ccache not found")
endif()

include(cmake/CPM.cmake)
CPMAddPackage("gh:godotengine/godot-cpp#4.0")

add_library(
  ${PROJECT_NAME} SHARED
  src/gdexample.cpp
  src/register_types.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC godot-cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
SET(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
