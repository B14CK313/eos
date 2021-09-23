# CMake setup
cmake_minimum_required(VERSION 3.16)
project(eos VERSION 0.0.1)

include(CheckIPOSupported)
include(cmake/CompilerFlagHelper.cmake)
include(cmake/CPM.cmake)

# Project setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MASTER_PROJECT ON)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
endif ()

option(EOS_TEST "Generate the test target." ${MASTER_PROJECT})

# OS detection
if (WIN32 OR CMAKE_BUILD_TYPE MATCHES .*Win)
    message("Compiling for Windows")
    set(OS win)
else ()
    set(OS unix)
endif ()

# Output dirs & compiler flags
if (CMAKE_BUILD_TYPE MATCHES Debug.*)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/${OS}/debug)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${OS}/debug)

    add_compile_definitions(DEBUG GLAD_DEBUG SPDLOG_ACTIVE_LEVEL=0)
    add_compile_options(-Og)

    enable_cxx_compiler_flag_if_supported("-Wall")
    enable_cxx_compiler_flag_if_supported("-Wextra")
    enable_cxx_compiler_flag_if_supported("-pedantic")
elseif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo.*)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/${OS}/relwithdebinfo)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${OS}/relwithdebinfo)

    add_compile_definitions(DEBUG SPDLOG_ACTIVE_LEVEL=0)
    add_compile_options(-O3)
else ()
    set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/${OS}/release)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/${OS}/release)

    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=2)
    add_compile_options(-O3)
endif ()

# Sources
set(EOS_SOURCES src/core/GameEngine.cpp include/eos/core/GameEngine.hpp include/eos/scene/GameState.hpp src/scene/StateManager.cpp include/eos/scene/StateManager.hpp src/utils.cpp include/eos/utils.hpp src/core/io/Config.cpp include/eos/core/io/Config.hpp src/Constants.cpp include/eos/Constants.hpp src/scene/resources/Shader.cpp include/eos/scene/resources/Shader.hpp src/stb_image.cpp src/scene/resources/Texture.cpp include/eos/scene/resources/Texture.hpp src/scene/3d/WorldObject.cpp include/eos/scene/3d/WorldObject.hpp src/core/ServiceProvider.cpp include/eos/core/ServiceProvider.hpp src/core/graphics/Window.cpp include/eos/core/graphics/Window.hpp src/scene/3d/ArcballCamera.cpp include/eos/scene/3d/ArcballCamera.hpp src/scene/3d/Camera.cpp include/eos/scene/3d/Camera.hpp src/scene/3d/PhysicsObject.cpp include/eos/scene/3d/PhysicsObject.hpp src/scene/3d/LightObject.cpp include/eos/scene/3d/LightObject.hpp src/core/ColorRGB.cpp include/eos/core/ColorRGB.hpp src/scene/resources/Font.cpp include/eos/scene/resources/Font.hpp src/scene/3d/GuiObject.cpp include/eos/scene/3d/GuiObject.hpp src/scene/resources/RegularTextureAtlas.cpp include/eos/scene/resources/RegularTextureAtlas.hpp include/eos/scene/resources/IrregularTextureAtlas.hpp src/scene/resources/IrregularTextureAtlas.cpp include/eos/scene/resources/TextureAtlas.hpp src/scene/resources/TextureAtlas.cpp src/core/ColorHSV.cpp include/eos/core/ColorHSV.hpp src/scene/resources/GradientText.cpp include/eos/scene/resources/GradientText.hpp src/scene/resources/Text.cpp include/eos/scene/resources/Text.hpp src/scene/GameState.cpp src/scene/resources/Material.cpp include/eos/scene/resources/Material.hpp include/eos/core/graphics/Graphics.hpp src/core/graphics/GraphicsOpenGL.cpp src/core/graphics/GraphicsVulkan.cpp)

# Load dependencies
find_package(Vulkan REQUIRED)
CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.3")
CPMAddPackage("gh:onqtam/doctest#2.4.6")
CPMAddPackage("gh:nemtrif/utfcpp@3.2.1")
CPMAddPackage("gh:boostorg/nowide#02f40f0b5f5686627fcddae93ff88ca399db4766")
CPMAddPackage("gh:gabime/spdlog@1.9.2")
CPMAddPackage("gh:fmtlib/fmt#8.0.1")
CPMAddPackage("gh:libsdl-org/SDL#release-2.0.16")
CPMAddPackage("gh:Dav1dde/glad@0.1.34")
if (OS MATCHES win)
    CPMAddPackage(
            NAME freetype
            GITHUB_REPOSITORY ubawurinna/freetype-windows-binaries
            VERSION 2.11.0
            DOWNLOAD_ONLY YES
    )
    if (freetype_ADDED)
        message("FREETYPE WINDOWS")
        add_library(freetype INTERFACE)
        target_include_directories(freetype INTERFACE ${freetype_SOURCE_DIR}/include)
    endif ()
else ()
    CPMAddPackage("gh:aseprite/freetype2#VER-2-10-0")
    if (freetype_ADDED)
        add_library(Freetype::Freetype ALIAS freetype)
    endif ()
endif ()
CPMAddPackage("gh:Chlumsky/msdfgen@1.9.1")
CPMAddPackage(
        NAME stb
        GITHUB_REPOSITORY nothings/stb
        GIT_TAG af1a5bc352164740c1cc1354942b1c6b72eacb8a
        DOWNLOAD_ONLY YES
)
if (stb_ADDED)
    add_library(stb INTERFACE)
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif ()

# Includes
#include_directories()

# Dependency configuration
set(OpenGL_GL_PREFERENCE GLVND)

# Setup compilation & linking
add_library(eos STATIC ${EOS_SOURCES} ${EOS_HEADERS})
add_library(eos::eos ALIAS eos)

target_link_libraries(eos PUBLIC utf8cpp nowide spdlog fmt SDL2-static glad Vulkan::Vulkan msdfgen::msdfgen msdfgen::msdfgen-ext freetype)
target_include_directories(eos PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if (ipo_supported)
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET eos PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else ()
    message(STATUS "IPO / LTO not supported: <${ipo_error}>")
endif ()


add_subdirectory(test)

# Add tests
if (EOS_TEST)
    add_subdirectory(test-game)
endif ()