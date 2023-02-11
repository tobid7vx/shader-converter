cmake_minimum_required(VERSION 3.22)

project(shaderconverter)

option(ENABLE_GLSLANG_BINARIES "Dont Want them" OFF)
option(SPIRV_CROSS_CLI "Dont want exe" OFF)

add_subdirectory(vendor/glslang2 glslang)
add_subdirectory(vendor/spirv-cross2 spirv-cross)

add_library(${PROJECT_NAME} source/ShaderConverter.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include spirv-cross glslang)

target_link_libraries(${PROJECT_NAME} PUBLIC
spirv-cross-core
spirv-cross-cpp
spirv-cross-glsl
spirv-cross-hlsl
spirv-cross-msl
spirv-cross-reflect
spirv-cross-util
glslang
#glslang-default-resource-limits
OGLCompiler
OSDependent
SPIRV
SPVRemapper
HLSL)