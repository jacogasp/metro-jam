cmake_minimum_required(VERSION 3.24.2)
project(CoreGame VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "lib")

option(CORE_GAME_ENABLE_PROFILER OFF)
if (CORE_GAME_ENABLE_PROFILER)
  message(STATUS "Enable profiler")
  add_compile_definitions(CORE_GAME_PROFILING)
endif ()

find_program(CCACHE ccache)
if (CCACHE)
  message(STATUS "ccache found")
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else ()
  message(STATUS "ccache not found")
endif ()

include(cmake/deps.cmake)

add_library(
        ${PROJECT_NAME} SHARED
        src/register_types.cpp
        src/logger.cpp
        src/main_scene.cpp
        src/player.cpp
        src/player_state.cpp
        src/world.cpp
        src/gate.cpp
        src/coin.cpp
        src/weapon.cpp
        src/chest.cpp
        src/damageable.cpp
        src/coin_spawner.cpp
        src/profiler.cpp
        src/persist.cpp
        src/io.cpp
        src/lifebar.cpp
        src/hud.cpp
        src/types.cpp
        src/player_commands.cpp
        src/bumblebee.cpp
        src/timer.cpp
        src/weapon.hpp
        src/wrench.cpp
        src/grenade.cpp
        src/macros.cpp
        src/poison_ranger.cpp
        src/health_bar.cpp
        src/state.cpp
        src/gun.cpp
        src/ray_cast.cpp
        src/superpowers.cpp
)

if (MINGW)
  message(STATUS Using MinGW)
  target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -static -pthread godot-cpp)
else ()
  target_link_libraries(${PROJECT_NAME} PRIVATE godot-cpp)
endif ()

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wcast-qual -Wformat=2 -Wundef -Werror=float-equal -Wshadow -Wcast-align -Wunused -Wnull-dereference -Wdouble-promotion -Wimplicit-fallthrough -Wextra-semi -Woverloaded-virtual -Wnon-virtual-dtor -Wold-style-cast)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
