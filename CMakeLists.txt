# CMakeLists.txt made by [destro](https://github.com/destroK503), thank him, otherwise I'd still
# using Makefiles lmao

cmake_minimum_required(VERSION 3.5.0)
project(Alisaml VERSION 0.1.0 LANGUAGES C)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)


add_executable(Alisaml src/main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
