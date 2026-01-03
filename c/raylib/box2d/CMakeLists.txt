cmake_minimum_required(VERSION 3.22)
project(sieve_simulation VERSION 1.0.0 LANGUAGES C)

include(FetchContent)

FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(box2d)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(sieve main.c)
target_link_libraries(sieve PRIVATE box2d raylib)
