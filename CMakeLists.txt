cmake_minimum_required(VERSION 3.13)
project(GLFW_GameEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/debug)
else ()
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/release)
endif ()
set(SOURCE_FILES src/main.cpp src/eos/GameEngine.cpp src/eos/GameEngine.hpp src/eos/IGameState.hpp src/eos/StateManager.cpp src/eos/StateManager.hpp src/game/states/MenuState.cpp src/game/states/MenuState.hpp src/game/states/IngameState.cpp src/game/states/IngameState.hpp src/game/states/GameStates.cpp src/game/states/GameStates.hpp)

if (WIN32)
    # ENV: BOOST_ROOT=I:\Programmieren\CCPP\Libraries\boost_1_70_0;BOOST_LIBRARYDIR=I:\Programmieren\CCPP\Libraries\boost_1_70_0\stage\lib;GLFW=I:\Programmieren\CCPP\Libraries\glfw-3.3;GLEW_ROOT=I:\Programmieren\CCPP\Libraries\glew-2.1.0
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME ON)
    set(Boost_ARCHITECTURE "-x64")

    # TODO: Switch to glbinding on Windows
    #set(GLEW_VERBOSE ON)
    #set(CMAKE_PREFIX_PATH $ENV{GLEW_ROOT}/include)
    #set(CMAKE_LIBRARY_PATH $ENV{GLEW_ROOT}/lib/Release/x64/)
else (WIN32)
    set(COMPILE_FLAGS "-DBOOST_LOG_DYN_LINK")
endif (WIN32)

find_package(Boost COMPONENTS log REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glbinding REQUIRED)
if (WIN32)
    find_library(GLFW_LIBRARIES glfw3 PATHS $ENV{GLFW} PATH_SUFFIXES "lib-mingw-w64")
    set(GLFW_INCLUDE_DIRS $ENV{GLFW}/include)
else (WIN32)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
endif (WIN32)

include_directories(${Boost_INCLUDE_DIR} ${GLFW_INCLUDE_DIRS})

add_definitions(${COMPILE_FLAGS})
add_executable(GLFW_GameEngine ${SOURCE_FILES})
target_link_libraries(GLFW_GameEngine PUBLIC ${Boost_LIBRARIES} ${GLFW_LIBRARIES} glbinding::glbinding glbinding::glbinding-aux OpenGL::GL)
