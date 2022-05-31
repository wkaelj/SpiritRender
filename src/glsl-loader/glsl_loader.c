#include "../glsl-loader/glsl_loader.h"

#include <shaderc/shaderc.h>
#include <utils/spirit_file.h>

// return SPIRIT_SHADER_TYPE_MAX on failure
static SpiritShaderType autoDetectShaderType(const char *path)
{
    const char *fileExtension = spStringStrip (path, '.');

    if (strncmp ("vert", fileExtension, 4))
    {
        return SPIRIT_SHADER_TYPE_VERTEX;
    }
    else if (strncmp ("frag", fileExtension, 4))
    {
        return SPIRIT_SHADER_TYPE_FRAGMENT;
    }
    else if (strncmp ("comp", fileExtension, 4))
    {
        return SPIRIT_SHADER_TYPE_COMPUTE;
    }
    else
        return SPIRIT_SHADER_TYPE_MAX;
}

extern SpiritShader loadCompiledShader(const char *path, SpiritShaderType type)
{

    u64 shaderCodeSize = 0;
    // get file size
    if (!(shaderCodeSize = spReadFileSize (path)))
    {
        return (SpiritShader){
            SPIRIT_SHADER_TYPE_AUTO_DETECT,
            (void *)0,
            0ll};
    }

    db_assert(shaderCodeSize, "shaderCodeSize was 0");

    void *shaderCodeBinary = alloc(shaderCodeSize);
    spReadFileBinary(shaderCodeBinary, path, &shaderCodeSize);

    SpiritShader out;
    out.type = type;
    out.shader = shaderCodeBinary;
    out.shaderSize = shaderCodeSize;

    return out;
}

// load a shader from glsl source code
extern SpiritShader loadSourceShader(
    const char *path,
    SpiritShaderType type)
{
    SpiritShader out = {};

    u32 filepathLength = 0;
    spPlatformLocalizeFileName(NULL, path, &filepathLength);
    char filepath[filepathLength];
    spPlatformLocalizeFileName(filepath, path, &filepathLength);

    
    // shader filename, without path
    const char *strippedShaderName = spStringStrip(path, SPIRIT_PLATFORM_FOLDER_BREAK);
    const u32 strippedShaderNameLength = strlen (strippedShaderName);

    // check if shader has been precompiled
    u32 shaderCodePathLength = (
        sizeof (GLSL_LOADER_CACHE_FOLDER) - 1 +
        strippedShaderNameLength +
        4); /* sizeof (".spv") - 1*/

    char shaderCodePath[shaderCodePathLength + 1];
    npf_snprintf(shaderCodePath, shaderCodePathLength + 1, "%s%s%s",
        GLSL_LOADER_CACHE_FOLDER,
        strippedShaderName,
        ".spv");

    log_verbose("Scanning for shader '%s'", shaderCodePath);

    if (spPlatformTestForFile(shaderCodePath) &&
        spReadFileModifiedTime(shaderCodePath) < spReadFileModifiedTime (filepath))
    {
        log_verbose ("Loading compiled shader '%s'",
            shaderCodePath);
        return loadCompiledShader(shaderCodePath, type);
    }
    else // is a else rather then guard statment to save memory
    {
        
        // compile the shader from source code

        if (!spReadFileExists(filepath))
        {
            return (SpiritShader) {0, 0, 0};
        }

        // get shader type
        shaderc_shader_kind shaderType;
        if (type == SPIRIT_SHADER_TYPE_VERTEX)
            shaderType = shaderc_vertex_shader;
        else if (type == SPIRIT_SHADER_TYPE_FRAGMENT)
            shaderType = shaderc_fragment_shader;
        else if (type == SPIRIT_SHADER_TYPE_COMPUTE)
        {
            log_error("Could not validate shader type '%d' loading shader '%s'",
                      type,
                      filepath);
            return (SpiritShader){0, 0, 0};
        }

        // attempt to automatically detect a shader based on file extensions
        // this is here so that it is obvous that you can intend for detection
        // to happen. It is not always a fallback case
        else if (SPIRIT_SHADER_TYPE_AUTO_DETECT || 1) // do no matter what
        {
            type = autoDetectShaderType(path);
            switch (type)
            {
            case SPIRIT_SHADER_TYPE_VERTEX:
                shaderType = shaderc_vertex_shader;
                break;
            case SPIRIT_SHADER_TYPE_FRAGMENT:
                shaderType = shaderc_fragment_shader;
                break;
            case SPIRIT_SHADER_TYPE_COMPUTE:
                shaderType = shaderc_compute_shader;
                break;
            default:
                log_error("Cannot detect type of shader '%s' \
                    based on file extension'",
                          filepath);
                return (SpiritShader) {0, 0, 0};
            }
        }

        // load source code
        size_t shaderSrcLength = spReadFileSize (filepath);
        char shaderSrc[shaderSrcLength];
        
        spReadFileText (shaderSrc, filepath, &shaderSrcLength);

        SpiritShader out = compileShader (
            shaderSrc, 
            shaderSrcLength, 
            strippedShaderName,
            type);

        // write output file
        log_verbose("Caching shader '%s'", shaderCodePath);

        // create folder
        char outputFolderPath[shaderCodePathLength];
        SpiritResult catchBuffer;
        catchBuffer = spStringTruncate(
            outputFolderPath,
            &shaderCodePathLength,
            shaderCodePath,
            SPIRIT_PLATFORM_FOLDER_BREAK,
            SPIRIT_TRUE);
        db_assert(catchBuffer == SPIRIT_SUCCESS, "Failed to truncate string");

        log_debug ("Attempting to create cache folder '%s'", outputFolderPath);
        catchBuffer = spWriteFileFolder (outputFolderPath);
        log_debug("Created Folder '%s'", outputFolderPath);

        db_assert (catchBuffer == SPIRIT_SUCCESS, "Failed to write folder");

        // write file
        catchBuffer = spWriteFileBinary(
            shaderCodePath,
            out.shader,
            out.shaderSize);
        log_debug("Wrote code to '%s'", shaderCodePath);
        db_assert(catchBuffer == SPIRIT_SUCCESS, "Failed to write file");
        log_verbose ("Compiled shader '%s'", shaderCodePath);

        return out;
    }
}

shaderc_shader_kind convertShaderType (SpiritShaderType type)
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
    }

    return shadercType;
}

extern SpiritShader compileShader (
    const char      *src,
    const u32        srcLength,
    const char      *outputShaderName,
    SpiritShaderType type)
{
    shaderc_shader_kind shadercType = convertShaderType(type);

    // initialize a shaderc compiler
    shaderc_compilation_result_t result = NULL;
    shaderc_compiler_t compiler = NULL;
    shaderc_compile_options_t settings = NULL;

    compiler = shaderc_compiler_initialize();
    db_assert(compiler, "Failure to create compiler");

    settings = shaderc_compile_options_initialize();
    db_assert(settings, "Failed to create shader settings");

    // optimize for performance
    shaderc_compile_options_set_optimization_level(
        settings,
        shaderc_optimization_level_performance);

    // set shader type
    shaderc_compile_options_set_target_env(
        settings,
        shaderc_target_env_vulkan,
        shaderc_env_version_vulkan_1_3);

    shaderc_compile_options_set_auto_map_locations(
        settings,
        false);

    SpiritShader out;

    // compile the shader
    result = shaderc_compile_into_spv(
        compiler,
        src,
        srcLength,
        shadercType,
        outputShaderName,
        "main",
        settings);
<<<<<<< Updated upstream
=======
    
    shaderc_compilation_status output;
    output = shaderc_result_get_compilation_status (result);
    if (output != shaderc_compilation_status_success)
    {
        log_error ("Could not compile shader '%s' because of error %u; Code:\n%s", outputShaderName, output, src);
        
        return (SpiritShader) {0, 0, 0};
    }
>>>>>>> Stashed changes

    size_t compiledShaderSize = shaderc_result_get_length(result);

    const void *compiledShader = shaderc_result_get_bytes(result);

    out.shader = compiledShader;
    out.shaderSize = compiledShaderSize;
    out.type = type;

    // release resources
    shaderc_result_release(result);
    shaderc_compile_options_release(settings);
    shaderc_compiler_release(compiler);

    return out;
}

// convert loaded shader to a VkShaderModule
// Try not to do this often, it is not a fast process
// Remember to destroy the shader module when you are done with them
// vkDestroyShaderModule (device, shader, callback (NULL))
extern VkShaderModule convertShaderToModule(
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
            SPIRIT_NULL,
            &out) != VK_SUCCESS)
        return SPIRIT_NULL;

    return out;
}