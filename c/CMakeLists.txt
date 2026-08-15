cmake_minimum_required(VERSION 3.24)

project(asteroids LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "Build raylib examples" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(raylib)

add_executable(asteroids main.c)
target_compile_features(asteroids PRIVATE c_std_99)
target_link_libraries(asteroids PRIVATE raylib)
