cmake_minimum_required(VERSION 3.5.0)
project(Alisama VERSION 0.1.0 LANGUAGES C)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

# Adding xml

FetchContent_Declare(
    xml
    GIT_REPOSITORY "https://github.com/ooxi/xml.c"
)

FetchContent_MakeAvailable(xml)


add_executable(Alisama src/main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_link_libraries(${PROJECT_NAME} PRIVATE xml)