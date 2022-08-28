#include "glsl_loader.h"

#include <shaderc/shaderc.h>
#include <utils/spirit_file.h>

const char GLSL_LOADER_CACHE_FOLDER[] = "glsl-loader-cache/";

// return SPIRIT_SHADER_TYPE_MAX on failure
static SpiritShaderType autoDetectShaderType(const char *path)
{

    u32 extensionLen = 5;
    char fileExtension[extensionLen];
    spStringStrip(fileExtension, &extensionLen, path, '.');

    if (strncmp("vert", fileExtension, 4))
    {
        return SPIRIT_SHADER_TYPE_VERTEX;
    }
    else if (strncmp("frag", fileExtension, 4))
    {
        return SPIRIT_SHADER_TYPE_FRAGMENT;
    }
    else if (strncmp("comp", fileExtension, 4))
    {
        return SPIRIT_SHADER_TYPE_COMPUTE;
    }
    else
        return SPIRIT_SHADER_TYPE_MAX;
}

shaderc_shader_kind convertShaderType(SpiritShaderType type)
{
    shaderc_shader_kind shadercType;
    switch (type)
    {
    case SPIRIT_SHADER_TYPE_VERTEX:
        shadercType = shaderc_vertex_shader;
        break;
    case SPIRIT_SHADER_TYPE_FRAGMENT:
        shadercType = shaderc_fragment_shader;
        break;
    case SPIRIT_SHADER_TYPE_COMPUTE:
        shadercType = shaderc_compute_shader;
        break;
    case SPIRIT_SHADER_TYPE_AUTO_DETECT:
        log_error("Must pass a valid shader type for shader compilation");
        shadercType = 0;
        break;
    default: // SPIRIT_SHADER_TYPE_MAX
        log_error("Must pass a valid shader type for shader compilation(not MAX)");
        shadercType = 0;
        break;
    }

    return shadercType;
}

extern SpiritShader spLoadCompiledShader(const char *path, SpiritShaderType type)
{

    u64 shaderCodeSize = spReadFileSize(path);
    // get file size
    if (shaderCodeSize < 3)
    {
        if (shaderCodeSize == 1)
            log_error("Cannot find file size of '%s'", path);
        if (shaderCodeSize == 0)
            log_error("Shader file '%s' does not exist", path);

        return (SpiritShader){
            SPIRIT_SHADER_TYPE_AUTO_DETECT,
            (void *)0,
            0ll, 0};
    }

    db_assert(shaderCodeSize != 0);

    void *shaderCodeBinary = malloc(shaderCodeSize);
    spReadFileBinary(shaderCodeBinary, path, shaderCodeSize);

    SpiritShader out;
    out.type = type;
    out.shader = shaderCodeBinary;
    out.shaderSize = shaderCodeSize;

    return out;
}

// load a shader from glsl source code
extern SpiritShader spLoadSourceShader(
    const char *path,
    SpiritShaderType type)
{
    // shader filename, without path
    u32 strippedShaderNameLength = 0;
    spStringStrip(NULL, &strippedShaderNameLength, path, SPIRIT_PLATFORM_FOLDER_BREAK);
    char strippedShaderName[strippedShaderNameLength];
    spStringStrip(
        strippedShaderName,
        &strippedShaderNameLength,
        path,
        SPIRIT_PLATFORM_FOLDER_BREAK);
    strippedShaderNameLength--; // remove space for terminator

    // check if shader has been precompiled
    u32 shaderCodePathLength =
        strlen(GLSL_LOADER_CACHE_FOLDER) +
        strippedShaderNameLength +
        5; /* sizeof(".spv\0")*/

    char shaderCodePath[shaderCodePathLength];
    snprintf(shaderCodePath, shaderCodePathLength, "%s%s%s",
        GLSL_LOADER_CACHE_FOLDER,
        strippedShaderName,
        ".spv");
    log_debug("Shader name: %s", shaderCodePath);

    if (spReadFileModifiedTime(shaderCodePath) > spReadFileModifiedTime(path))
    {
        log_verbose("Loading compiled shader '%s'",
                    shaderCodePath);
        return spLoadCompiledShader(shaderCodePath, type);
    }
    else
    {

        if (SPIRIT_SHADER_TYPE_AUTO_DETECT) // do no matter what
        {
            type = autoDetectShaderType(path);
        }

        // load source code
        size_t shaderSrcLength = spReadFileSize(path);

        if (shaderSrcLength == 0)
        {
            log_error("Shader '%s' does not exist", path);
            return (SpiritShader){0, 0, 0, 0};
        }

        char shaderSrc[shaderSrcLength + 1];

        spReadFileText(shaderSrc, path, shaderSrcLength + 1);

        SpiritShader out = spCompileShader(
            shaderSrc,
            shaderSrcLength,
            strippedShaderName,
            type);

        if (out.shaderSize == 0)
        {
            return out;
        }

        log_verbose("Compiled shader '%s'", shaderCodePath);

        // create folder
        SpiritResult catchBuffer = spWriteFileFolder(GLSL_LOADER_CACHE_FOLDER);
        if (catchBuffer)
            log_error("Failed to create folder '%s'",
                      GLSL_LOADER_CACHE_FOLDER);

        db_assert_msg(catchBuffer == SPIRIT_SUCCESS, "Failed to write folder");

        // write file
        catchBuffer = spWriteFileBinary(
            shaderCodePath,
            out.shader,
            out.shaderSize);
        if (catchBuffer)
            log_error("Failed to file '%s'",
                      shaderCodePath);

        db_assert_msg(catchBuffer == SPIRIT_SUCCESS, "Failed to write file");

        log_verbose("Cached shader '%s' with size %lu", shaderCodePath, out.shaderSize);

        return out;
    }
}

extern SpiritShader spCompileShader(
    const char *src,
    const u32 srcLength,
    const char *outputShaderName,
    SpiritShaderType type)
{
    shaderc_shader_kind shadercType = convertShaderType(type);

    // initialize a shaderc compiler
    shaderc_compilation_result_t result = NULL;
    shaderc_compiler_t compiler = NULL;
    shaderc_compile_options_t settings = NULL;

    compiler = shaderc_compiler_initialize();
    db_assert_msg(compiler, "Failure to create compiler");

    settings = shaderc_compile_options_initialize();
    db_assert_msg(settings, "Failed to create shader settings");

    // set shader type
    shaderc_compile_options_set_target_env(
        settings,
        shaderc_target_env_vulkan,
        shaderc_env_version_vulkan_1_0);

    // compile the shader
    result = shaderc_compile_into_spv(
        compiler,
        src,
        srcLength,
        shadercType,
        outputShaderName,
        "main",
        NULL /*settings*/);

    shaderc_compilation_status output;
    output = shaderc_result_get_compilation_status(result);
    if (output != shaderc_compilation_status_success)
    {
        log_error("Could not compile shader '%s' because of error %u\nError: %s\nCode:\n%s", outputShaderName, output, shaderc_result_get_error_message(result), src);
        shaderc_result_release(result);
        shaderc_compile_options_release(settings);
        shaderc_compiler_release(compiler);

        return (SpiritShader){0, 0, 0, 0};
    }

    size_t compiledShaderSize = shaderc_result_get_length(result);
    db_assert_msg(compiledShaderSize, "compiled shader size cannot be 0");

    SpiritShaderCode compiledShader = (SpiritShaderCode)malloc(compiledShaderSize);

    const SpiritShaderCode shadercResult = (u32 *)shaderc_result_get_bytes(result);

    compiledShader = memcpy(
        compiledShader,
        shadercResult,
        compiledShaderSize);

    db_assert_msg(memcmp(
                  compiledShader,
                  shadercResult,
                  compiledShaderSize) == 0,
              "Shader did not copy)");

    SpiritShader out = {};
    out.shader = compiledShader;
    out.shaderSize = compiledShaderSize;
    db_assert_msg(out.shaderSize == compiledShaderSize, "Shader sizes do not match");
    out.type = type;

    // release resources
    shaderc_result_release(result);
    shaderc_compile_options_release(settings);
    shaderc_compiler_release(compiler);

    return out;
} // compileShader

// convert loaded shader to a VkShaderModule
// Try not to do this often, it is not a fast process
// Remember to destroy the shader module when you are done with them
// vkDestroyShaderModule (device, shader, callback (NULL))
extern VkShaderModule spConvertShaderToModule(
    SpiritDevice device,
    SpiritShader *shader)
{

    VkShaderModuleCreateInfo moduleInfo = {};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.codeSize = shader->shaderSize;
    moduleInfo.pCode = shader->shader;

    VkShaderModule out;
    if (vkCreateShaderModule(
            device->device,
            &moduleInfo,
            NULL,
            &out) != VK_SUCCESS)
        return NULL;

    return out;
}

SpiritResult spDestroyShader(SpiritShader shader)
{
    free(shader.shader);
    return SPIRIT_SUCCESS;
}
