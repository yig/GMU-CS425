cmake_minimum_required(VERSION 3.12...3.25)
project(
    stepbystep-secret
    LANGUAGES CXX C
)

option(USE_DEAR_IMGUI "Use Dear ImGUI" OFF)
option(USE_SOLOUD "Use soloud" OFF)
set(WEBGPU_BACKEND "wgpu" CACHE STRING "WebGPU backend (wgpu or dawn)")

## Load packages
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    )
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable( glfw3 )

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    )
FetchContent_MakeAvailable( glm )

FetchContent_Declare(
  webgpu
  GIT_REPOSITORY https://github.com/eliemichel/WebGPU-distribution
  ## Can change `wgpu` to `dawn`
  GIT_TAG        ${WEBGPU_BACKEND}
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable( webgpu )

FetchContent_Declare(
  glfw3webgpu
  GIT_REPOSITORY https://github.com/eliemichel/glfw3webgpu
  GIT_TAG        main
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
FetchContent_Populate(glfw3webgpu)
FetchContent_GetProperties(glfw3webgpu)
add_subdirectory(${glfw3webgpu_SOURCE_DIR})

if( USE_SOLOUD )
    FetchContent_Declare(
      soloud
      GIT_REPOSITORY https://github.com/jarikomppa/soloud
      GIT_TAG        master
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    FetchContent_Populate(soloud)
    FetchContent_GetProperties(soloud)
    # add_subdirectory(${soloud_SOURCE_DIR}/contrib)
    file(GLOB soloud_sources "${soloud_SOURCE_DIR}/src/audiosource/*/*.c*" "${soloud_SOURCE_DIR}/src/c_api/*.c*" "${soloud_SOURCE_DIR}/src/core/*.c*" "${soloud_SOURCE_DIR}/src/filter/*.c*" "${soloud_SOURCE_DIR}/src/backend/miniaudio/soloud_miniaudio.cpp")
    add_library(soloud ${soloud_sources})
    target_compile_definitions(soloud PRIVATE WITH_MINIAUDIO MA_NO_RUNTIME_LINKING)
    target_include_directories(soloud PUBLIC "${soloud_SOURCE_DIR}/include")
    if(APPLE)
        find_library(AudioUnit_LIBRARY AudioUnit)
        find_library(CoreAudio_LIBRARY CoreAudio)
        target_link_libraries(soloud INTERFACE ${AudioUnit_LIBRARY} ${CoreAudio_LIBRARY})
    endif()
endif()

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb/
  GIT_TAG        master
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)
FetchContent_Populate(stb)
FetchContent_GetProperties(stb)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog/
    GIT_TAG v1.x
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    )
FetchContent_MakeAvailable( spdlog )

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

if( USE_DEAR_IMGUI )
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.91.1
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        )
    # FetchContent_MakeAvailable( imgui )
    FetchContent_Populate( imgui )
    FetchContent_GetProperties( imgui )
    file(GLOB imgui_sources "${imgui_SOURCE_DIR}/*.c*" "${imgui_SOURCE_DIR}/backends/imgui_impl_wgpu.cpp*" "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp*")
    add_library(imgui ${imgui_sources})
    target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}" "${imgui_SOURCE_DIR}/backends")
    target_link_libraries(imgui PUBLIC glfw webgpu)
    set_target_properties(imgui PROPERTIES CXX_STANDARD 17 )
endif()

## Declare the engine library
add_library(illengine STATIC
    src/Engine.cpp
    src/GraphicsManager.cpp
    src/InputManager.cpp
    src/ResourceManager.cpp
    src/SoundManager.cpp
    src/ScriptManager.cpp
    src/GUIManager.cpp
    )
set_target_properties(illengine PROPERTIES CXX_STANDARD 20 )
target_include_directories(illengine PUBLIC src)
target_link_libraries(illengine PUBLIC glfw glm::glm spdlog::spdlog webgpu glfw3webgpu stb sol2 lua_static)
if( USE_DEAR_IMGUI )
    target_link_libraries(illengine PUBLIC imgui)
endif()
if( USE_SOLOUD )
    target_link_libraries(illengine PUBLIC soloud)
endif()


## Declare the game executable
add_executable(helloworld demo/helloworld.cpp)
set_target_properties(helloworld PROPERTIES CXX_STANDARD 20 )
target_link_libraries(helloworld PRIVATE illengine)
target_copy_webgpu_binaries(helloworld)
