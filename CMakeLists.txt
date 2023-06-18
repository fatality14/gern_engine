cmake_minimum_required(VERSION 3.5)

project(ogl LANGUAGES CXX)

set(CCACHE_PATH D:/AYNP/ccache46_64)
set(CCACHE_EXE ccache.exe)

find_program(CCACHE_FOUND ccache ${CCACHE_PATH})
if(CCACHE_FOUND)
    #set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PATH}/${CCACHE_EXE}")
    message("using ccache")
endif(CCACHE_FOUND)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")

set(CMAKE_FIND_LIBRARY_PREFIXES "" "lib")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll" ".a" ".so")

add_executable(ogl main.cpp)

target_compile_options(ogl PRIVATE -Wall -Wextra -pedantic)
target_include_directories(ogl PRIVATE
    engine/
    thirdparty/GL_H/
    thirdparty/GLFW/
    thirdparty/SOIL2/
    thirdparty/glm/
    libs/
    usrsrc/
)

#[[ use on your own risk
file(GLOB ALL_HEADERS
     "engine/animation/*.h"
     "engine/buffer/*.h"
     "engine/common/*.h"
     "engine/mesh/*.h"
     "engine/object/*.h"
     "engine/shader/*.h"
     "engine/texture/*.h"
)

target_precompile_headers(ogl PRIVATE
    <chrono>
    <type_traits>
    <unordered_set>
    <unordered_map>
    <queue>
    <iostream>
    <fstream>
    <vector>
    ${ALL_HEADERS})
]]

message(${CMAKE_SYSTEM_NAME})
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_library(GLEW32 libGLEW.so.2.2 libs/linux)
    find_library(GLFW3 glfw3 libs/linux)
    find_library(SOIL libsoil2 libs/linux)
    find_library(OGL opengl libs/linux)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    find_library(GLEW32 glew32 libs/windows)
    find_library(GLFW3 glfw3 libs/windows)
    find_library(SOIL libsoil2 libs/windows)
    find_library(OGL opengl32 libs/windows)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")


#find_package(OpenGL REQUIRED)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(ogl PRIVATE stdc++)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
target_link_libraries(ogl LINK_PUBLIC ${OGL} ${SOIL} ${GLEW32} ${GLFW3})
