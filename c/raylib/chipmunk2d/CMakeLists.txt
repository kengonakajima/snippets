cmake_minimum_required(VERSION 3.22)
project(sieve_chipmunk VERSION 1.0.0 LANGUAGES C)

include(FetchContent)

# Save original flags and remove -ffast-math for Chipmunk2D
set(SAVED_CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

FetchContent_Declare(
    chipmunk
    GIT_REPOSITORY https://github.com/slembcke/Chipmunk2D.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
set(BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC ON CACHE BOOL "" FORCE)
set(INSTALL_STATIC OFF CACHE BOOL "" FORCE)
set(FORCE_CLANG_BLOCKS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(chipmunk)

# Add -fno-fast-math to override -ffast-math
target_compile_options(chipmunk_static PRIVATE -fno-fast-math)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(sieve main.c)
target_link_libraries(sieve PRIVATE chipmunk_static raylib)
target_include_directories(sieve PRIVATE ${chipmunk_SOURCE_DIR}/include)
