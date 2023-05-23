cmake_minimum_required(VERSION 3.24.2)
project(CoreGame VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(GODOT_CUSTOM_API_FILE "${CMAKE_SOURCE_DIR}/extension_api.json")

find_program(CCACHE ccache)
if (CCACHE)
    message(STATUS "ccache found")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else ()
    message(STATUS "ccache not found")
endif ()

include(cmake/CPM.cmake)
CPMAddPackage("gh:godotengine/godot-cpp#4.0")

add_library(
        ${PROJECT_NAME} SHARED
        src/register_types.cpp
        src/coin.cpp
        src/gdexample.cpp
        src/logger.cpp
        src/main_scene.cpp
        src/player.cpp
        src/player_state.cpp
)


if (MSVC)
    target_link_libraries(${PROJECT_NAME} -static godot-cpp)
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE godot-cpp)
endif ()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
SET(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
