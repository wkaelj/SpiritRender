#include "../glsl-loader/glsl_loader.h"

#include <shaderc/shaderc.h>
#include <stdio.h>

extern SpiritShader loadCompiledShader (const char *path, SpiritShaderType type) {

    // open shader readonly
    FILE *shaderSrc = fopen (path, "r");

    fseek (shaderSrc, 0, SEEK_END);
    u64 fileSize = ftell (shaderSrc);

    fseek (shaderSrc, 0, SEEK_SET);

    void *shaderBinary = alloc (fileSize);
    fread (shaderBinary, fileSize, 1, shaderSrc);

    SpiritShader out;
    out.type =       type;
    out.shader =     shaderBinary;
    out.shaderSize = fileSize;
    out.shaderPath = path;

    fclose (shaderSrc);
    return out;

}

// load a shader from glsl source code
extern SpiritShader loadSourceShader (const char *filepath, SpiritShaderType type) {
    SpiritShader out = {};

    const char *shaderSrc;
    FILE *shaderSrcFile = NULL;
    size_t shaderSrcLength;
    // shader filename, without path
    char *strippedShaderName = spStringStrip (filepath, '/');

    // check if shader has been precompiled
    char *shaderCodePath;
    sprintf (shaderCodePath, "%s%s%s",
    GLSL_LOADER_CACHE_FOLDER,
    strippedShaderName,
    ".spv");

    FILE *shaderCodeFile = fopen(shaderCodePath, "r");
    if (shaderCodeFile) {

        fseek (shaderCodeFile, 0, SEEK_END);
        size_t shaderCodeSize = ftell (shaderCodeFile);
        fseek (shaderCodeFile, 0, SEEK_SET);

        SpiritShader out;
        out.type = type;
        out.shaderPath = shaderCodePath;
        void *readShaderBinary = alloc (shaderCodeSize);
        out.shaderSize = fread (
            readShaderBinary,
            shaderCodeSize,
            1, // number of array elements to divide shader into
            shaderCodeFile);
        out.shader = readShaderBinary;

        fclose (shaderCodeFile);

        return out;
    }
    else
    { // otherwise compile shader

        shaderSrcFile = fopen (filepath, "r");
        if (!shaderSrcFile) {
            log_error("Failed to open shader source file '%s'", filepath);
            goto failure;
        }

        fseek (shaderSrcFile, 0, SEEK_END);
        shaderSrcLength = ftell (shaderSrcFile);

        // initialize a shaderc compiler
        shaderc_compilation_result_t result = NULL;
        shaderc_compiler_t compiler =         NULL;
        shaderc_compile_options_t settings =  NULL;

        compiler = shaderc_compiler_initialize();
        db_assert(compiler, "Failure to create compiler");

        settings = shaderc_compile_options_initialize ();
        db_assert(settings, "Failed to create shader settings");

        // optimize for performance
        shaderc_compile_options_set_optimization_level (
            settings, 
            shaderc_optimization_level_performance);
        
        // set shader type
        shaderc_compile_options_set_target_env (
            settings, 
            shaderc_target_env_vulkan, 
            shaderc_env_version_vulkan_1_3);

        shaderc_compile_options_set_auto_map_locations(
            settings,
            false);
        
        shaderc_shader_kind shaderType;
        if (type == SPIRIT_SHADER_TYPE_VERTEX)
            shaderType = shaderc_vertex_shader;
        else if (type == SPIRIT_SHADER_TYPE_FRAGMENT)
            shaderType = shaderc_fragment_shader;
        else if (type == SPIRIT_SHADER_TYPE_COMPUTE) {
            log_error ("Could not invalid shader type '%d' loading shader '%s'",
                type,
                filepath);
            goto failure;
        }
        // attempt to automatically detect a shader based on file extensions
        else if(SPIRIT_SHADER_TYPE_AUTO_DETECT || 1)
        {
            const char *fileExtension = spStringStrip (filepath, '.');
            // I do this because switch only supports integral types
            switch (fileExtension[0]) {
                case 'v' /* vert */:
                    shaderType = shaderc_vertex_shader;
                    type = SPIRIT_SHADER_TYPE_VERTEX;
                    break;
                case 'f' /* frag */:
                    shaderType = shaderc_fragment_shader;
                    type = SPIRIT_SHADER_TYPE_FRAGMENT;
                    break;
                case 'c' /* compute */:
                    shaderType = shaderc_compute_shader;
                    type = SPIRIT_SHADER_TYPE_COMPUTE;
                    break;
                default:
                    log_error ("Cannot detect type of shader '%s' \
                    based on file extension '%s'",
                    fileExtension,
                    filepath);
                    goto failure;
            }
        }

        result = shaderc_compile_into_spv(
            compiler,
            shaderSrc,
            shaderSrcLength,
            shaderType,
            strippedShaderName,
            "main",
            settings);
        
        size_t compiledShaderSize = shaderc_result_get_length (result);

        const void *compiledShader = shaderc_result_get_bytes (result);

        // write result to spv file
        char *outputFilepath;
        sprintf (outputFilepath, "%s%s%s",
            GLSL_LOADER_CACHE_FOLDER,
            strippedShaderName,
            ".spv");
        FILE *outputFile = fopen (outputFilepath, "w");
        if (!outputFile) {
            log_error ("Cannot write shader '%s', compiled from src '%s'",
                outputFilepath,
                filepath);
            goto failure;
        }

        fwrite (compiledShader, compiledShaderSize, 1, outputFile);
        fclose (outputFile);
        outputFile = NULL;
        
        // release resources
        shaderc_result_release (result);
        shaderc_compile_options_release (settings);
        shaderc_compiler_release (compiler);

        out.type =       type;
        out.shader =     compiledShader;
        out.shaderSize = compiledShaderSize;
        out.shaderPath = outputFilepath;


        return out;
        
        failure:
        // release resources
        if (shaderSrcFile)
            fclose(shaderSrcFile);
        if (outputFile)
            fclose(outputFile);
        if (result)
            shaderc_result_release(result);
        if (settings)
            shaderc_compile_options_release(settings);
        if (compiler)
            shaderc_compiler_release(compiler);
        SpiritShader fail = {
            SPIRIT_SHADER_TYPE_MAX,
            NULL,
            0,
            NULL};
        return fail;
        }
}

// convert loaded shader to a VkShaderModule
// Try not to do this often, it is not a fast process
// Remember to destroy the shader module when you are done with them
// vkDestroyShaderModule (device, shader, callback (NULL))
extern VkShaderModule convertShaderToModule (
    SpiritDevice device,
    SpiritShader *shader) {

    VkShaderModuleCreateInfo moduleInfo = {};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.codeSize = shader->shaderSize;
    moduleInfo.pCode = shader->shader;
    
    VkShaderModule out;
    if (vkCreateShaderModule (
        device->device, 
        &moduleInfo, 
        SPIRIT_NULL, 
        &out) != VK_SUCCESS) return SPIRIT_NULL;
    
    return out;
}