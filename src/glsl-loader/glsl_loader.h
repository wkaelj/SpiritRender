#pragma once
#include <spirit_header.h>
#include <core/spirit_device.h>

// Functions to load and compile glsl vertex and fragment shader
// Supports compiled and uncompiled shaders
// Automaticaly stores compiled shaders a cache next to the executable
// 
// All filepaths that do not start with ./ or / will be relative to the
// executable file, not the CWD. This is to make finding program files
// more intuitive.
// 
// 
// Kael Johnston April 25 2022

// file to store cached shaders, relative to the executable
#define GLSL_LOADER_CACHE_FOLDER "glsl-loader-cache/"

// loading functions

// load a .spv shader, compiled to SPIR-V
extern SpiritShader spLoadCompiledShader (
    const char      *path, 
    SpiritShaderType type);

// load a shader from glsl source code
// if the function failed, all the outputs will be 0
// for example, if the shader pointer is NULL, it likely didn't work
// function has MANY side effect
//  - Creates cached shader files
//  - Compiles shader source code
//  - does not compile the shader of cache files exist
extern SpiritShader spLoadSourceShader (
    const char      *filepath, 
    SpiritShaderType type);

// compile a shader from glsl source code string
extern SpiritShader spCompileShader(
    const char      *src,
    const u32        srcLength,
    const char      *outputShaderName,
    SpiritShaderType type);

// use functions

// convert loaded shader to a VkShaderModule
extern VkShaderModule spConvertShaderToModule (
    SpiritDevice device, 
    SpiritShader *shader);

extern SpiritResult spDestroyShader(SpiritShader);
