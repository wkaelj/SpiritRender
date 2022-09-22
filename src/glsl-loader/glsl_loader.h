#pragma once
#include <spirit_header.h>
#include <render/spirit_device.h>

/**
 * Functions to load and compile glsl vertex and fragment shader
 * Supports compiled and uncompiled shaders
 * Automaticaly stores compiled shaders a cache next to the executable
 *
 * All filepaths that do not start with ./ or / will be relative to the
 * executable file, not the CWD. This is to make finding program files
 * more intuitive.
 *
 * @author Kael Johnston
 * @date Jul 24 2022
 */

// file to store cached shaders, relative to the executable
extern const char GLSL_LOADER_CACHE_FOLDER[];

/**
 * Load a compiled SPIR-V shader. You can compile a GLSL shader to SPIR-V using
 * the glslc compiler.
 *
 * @param path the path to the SPIR-V (.spv) binary
 * @param type the type of shader, vertex, fragment, etc.
 *
 * @return a SpiritShader object containing the shader. The shader modules
 * shaderSize will be set to 0 for failure.
 *
 * @author Kael Johnston
 */
SpiritShader spLoadCompiledShader(const char *path, SpiritShaderType type);

/**
 * Loads a GLSL shader from source code. It will automatically store the
 * compiled shader in the GLSL_LOADER_CACHE_FOLDER so that it will not
 * need to be compiled again in the future. It will automatically recompile
 * and recache the shader if the source file was modified after the binary
 * was compiled.
 *
 * @param filepath the shader source file
 * @param type the type of shader, vertex, fragment or compute
 *
 * @return a SpiritShader object, which can be converted to a VkShaderModule
 * using the spConvertShaderToModule function
 *
 * @author Kael Johnston
 */
SpiritShader spLoadSourceShader(const char *filepath, SpiritShaderType type);

/**
 * Compile a shader from a string of source. It will return a
 * SpiritShader object storing the code.
 *
 * If compilation fails the returned SpiritShader's shaderSize will be set to 0
 *
 * @param src the shader source code
 * @param srcLength the number of characters the source code is long
 * @param outputShaderName the name of the shader, for the shader module
 * @param type the type of shader, vertex, fragment, etc.
 *
 * @return a SpiritShader object containing the shader. If the shadersize is 0,
 * it means that compilation failed.
 *
 * @author Kael Johnston
 */
SpiritShader spCompileShader(
    const char *src,
    const u32 srcLength,
    const char *outputShaderName,
    SpiritShaderType type);

/**
 * Used to convert a SpiritShader to a vulkan VkShaderModule.
 *
 * @param must be a valid SpiritDevice, created using spCreateDevice()
 * @param shader must be a pointer to a valid SpiritShader, loaded or compiled
 * using one of the functions in this file.
 *
 * @return a VkShaderModule that can be used. It must be destroyed by the user.
 * The associated SpiritShader can be destroyed without breaking the shader
 * module, but the device cannot. It will return NULL for a failure.
 *
 * @author Kael Johnston
 */
VkShaderModule
spConvertShaderToModule(const SpiritDevice device, SpiritShader *shader);

/**
 * Destroy a SpiritShader.
 *
 * @param shader the shader which will be destroyed.
 *
 * @return whether or not the destruction was successful. Even if destruction
 * fails, the shader cannot be used afterwards.
 *
 * @author Kael Johnston
 */
SpiritResult spDestroyShader(SpiritShader shader);
