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
    libs/)

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

find_library(GLEW32 glew libs/linux)
find_library(GLFW3 glfw3 libs/linux)
find_library(SOIL libsoil2 libs/linux)
find_library(OGL opengl libs/linux)

#find_package(OpenGL REQUIRED)

target_link_libraries(ogl LINK_PUBLIC ${OGL} ${SOIL} ${GLEW32} ${GLFW3})
