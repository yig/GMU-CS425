cmake_minimum_required(VERSION 3.14...3.30)
project(
    snippets # Name this whatever you want
    LANGUAGES CXX C
)

## Enable decentralized package loading
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    lua
    GIT_REPOSITORY https://github.com/walterschell/Lua
    GIT_TAG v5.4.5
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    )
set(LUA_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable( lua )

FetchContent_Declare(
    sol2
    GIT_REPOSITORY https://github.com/ThePhD/sol2
    GIT_TAG v3.3.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    )
FetchContent_MakeAvailable( sol2 )


## Declare the snippets
## Set a global C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## Single file snippets
add_executable( helloworld demo/helloworld.cpp )
add_executable( templates demo/templates.cpp )
add_executable( callbacks demo/callbacks.cpp )
add_executable( callbacks_with_parameters demo/callbacks_with_parameters.cpp )
add_executable( chrono demo/chrono_sleep_for.cpp )
add_executable( friend demo/friend.cpp )
add_executable( constructor_reference demo/constructor_reference.cpp )
add_executable( pimpl pimpl/house.cpp pimpl/main.cpp )
add_executable( globals globals/globals.cpp globals/globals_main.cpp )
add_executable( entity_get demo/entity_get.cpp )

## How to create and use a library
add_library( disco STATIC disco/ballroom.cpp )
target_include_directories( disco PUBLIC "disco" )
add_executable( disco_demo demo/disco_demo.cpp )
target_link_libraries( disco_demo PRIVATE disco )

## How to set the working directory when running automatically
add_executable( paths demo/paths.cpp )
add_custom_target( run_paths paths WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )

## Example uses lua
add_executable( lua_parameters demo/lua_parameters.cpp )
target_link_libraries( lua_parameters PRIVATE sol2 lua_static )

## Example uses lua
add_executable( lua_globals demo/lua_globals.cpp )
target_link_libraries( lua_globals PRIVATE sol2 lua_static )
