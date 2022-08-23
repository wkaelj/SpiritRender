// test main function

#include <spirit_header.h>
#include <unistd.h>

// types
#include "core/spirit_context.h"

#include "core/spirit_mesh.h"
#include "core/spirit_material.h"

// utils
#include "core/spirit_types.h"
#include "glsl-loader/glsl_loader.h"
#include "utils/spirit_file.h"
#include "utils/platform.h"

void mainlooptest (void);

// run all the tests
void Test(void);


int main (int argc, char **argv) {


    spPlatformSetExecutableFolder(argv[0]);

    if (argc >= 2 && strcmp("-h", argv[1]) == 0)
    {
        printf(
            "Usage:\n\t"
            "--test - run the tests\n\t"
            "--delete-shader-cache - delete the shader cache\n");
    }

    // tests
    if (argc >= 2 && strcmp("--test", argv[1]) == 0)
    {
        Test();
        return 0;
    }

    if (argc >= 2 && strcmp("--delete-shader-cache", argv[1]) == 0)
    {
        spPlatformDeleteFolder(GLSL_LOADER_CACHE_FOLDER);
        return 0;
    }

    mainlooptest();

    return 0;
}

void mainlooptest (void) {

    SpiritContextCreateInfo contextInfo = {};
    contextInfo.enableValidation = true;
    contextInfo.windowName = "Hi Square :D";
    contextInfo.windowSize = (SpiritResolution) {800, 600};
    contextInfo.windowFullscreen = false;

    struct FunctionTimerData startup = start_timer("startup");
    SpiritContext context = spCreateContext(&contextInfo);

    log_debug("Context res %lux%lu",
        context->screenResolution.w,
        context->screenResolution.h);

    if (context == NULL)
        return;

    // SpiritMaterialCreateInfo materialInfo = {};
    // materialInfo.name = "std";
    // materialInfo.fragmentShader = "shaders/simple_shader.frag";
    // materialInfo.vertexShader = "shaders/simple_shader.vert";

    SpiritMaterialCreateInfo materialInfo = {
        .name = "std",
        .fragmentShader = "shaders/simple_shader.frag",
        .vertexShader = "shaders/simple_shader.vert"
    };

    SpiritMaterial material = spCreateMaterial(
        context,
        &materialInfo);

    if (material == NULL)
        return;

    spContextAddMaterial(context, material);

    vec3 meshVerts[] = {
        {-0.5, -0.5f, 0.0f},
        {-0.5f, 0.5f, 0.0f},
        {0.5f, -0.5f, 0.0f},
        {0.5f, -0.5f, 0.0f},
        {-0.5f, 0.5f, 0.0f},
        {0.5f, 0.5f, 0.0f}
    };

    SpiritMeshCreateInfo meshInfo = {};
    meshInfo.vertCount = 6;
    meshInfo.verts = meshVerts;

    SpiritMesh mesh = spCreateMesh(context, &meshInfo);

    SpiritMeshManager meshManager = spCreateMeshManager(context, NULL);
    const SpiritMeshReference meshRef = spMeshManagerAddMesh(meshManager, mesh);

    end_timer(startup);

    SpiritWindowState windowState;

    u64 frameIndex = 0; // check the current frame being rendered
    while ((windowState = spWindowGetState(context->window)) != SPIRIT_WINDOW_CLOSED)
    {

        struct FunctionTimerData frametime = start_timer("frametime");
        // handle window resized
        if (windowState == SPIRIT_WINDOW_RESIZED)
        {
            if(spContextHandleWindowResized(context)) continue;
        } else if (windowState == SPIRIT_WINDOW_RESIZING) continue;

        // time_function(spMaterialAddMesh(material, meshRef));

        SpiritResult result = SPIRIT_SUCCESS;
        time_function_with_return(spContextSubmitFrame(context), result);

        if (result)
        {
            log_error("Error on frame %lu", frameIndex);
            sleep(5);
        }

        end_timer(frametime);

        ++frameIndex;

    }

    spDeviceWaitIdle(context->device);
    spDestroyMeshManager(context, meshManager);
    spDestroyMaterial(context, material);
    spDestroyContext(context);

}

//
// Tests
//

void runTest(bool test)
{
    static u32 testIntex;
    if(test)
    {
        log_info("Test %u passed.", testIntex);
    } else
    {
        log_error("Test %u failed.", testIntex);
    }

    testIntex++;
}

// tests
bool TestPlatformLocalizeFilename(const char *input, const char *expected)
{

    u32 max = 0;
    time_function(spPlatformLocalizeFileName(NULL, input, &max));
    char output[max];
    time_function(spPlatformLocalizeFileName(output, input, &max));
    if (strcmp(output, expected))
    {
        log_error("Expected %s, got %s", expected, output);
        return false;
    }
    return true;
}

bool TestStringTruncate(const char *input, const char slicer, bool inclusive, const char *expected)
{

    u32 len = strlen(input) + 1;
    char dest[len];

    time_function(spStringTruncate(dest, &len, input, slicer, inclusive));

    if (strcmp(dest, expected))
    {
        log_error("Expected %s, got %s", expected, dest);
        return false;
    }
    return true;
}

bool TestStringStrip(const char *input, const char slicer, const char *expected)
{

    u32 len = strlen(input) + 1;
    char dest[len];

    time_function(spStringStrip(dest, &len, input, slicer));

    if (strcmp(dest, expected))
    {
        log_error("Expected %s, got %s", expected, dest);
        return false;
    }
    return true;
}

/**
 * @brief Test the built in file utilities. The function will create a file,
 * write text to it, read the file and verify the contents. It then deletes the file.
 * NOTE: this test will fail if the file is inside a folder that doesn't exist.
 *
 * @param textFileName
 * @param testFileContents
 * @return true
 * @return false
 */
bool TestFileUtilities(
    const char *restrict testFileName,
    const char *restrict testFileContents)
{

    SpiritResult success = 0;

    u64 testFileContentsLength = strlen(testFileContents) + 1;
    char buf[testFileContentsLength]; // declare buf for goto


    time_function_with_return(spWriteFileBinary(testFileName, testFileContents, testFileContentsLength - 1), success);
    if (success) return false;

    time_function_with_return(spReadFileText(buf, testFileName, testFileContentsLength), success);
    if (success) goto failure;

    int cmpResult;
    time_function_with_return(strncmp(buf, testFileContents, testFileContentsLength), cmpResult);
    if (cmpResult != 0)
    {
        log_error("input string and read string do not match");
        goto failure;
    }

    memset(buf, 0, testFileContentsLength);
    time_function_with_return(spReadFileBinary(buf, testFileName, testFileContentsLength), success);
    if (success) goto failure;
    cmpResult = strncmp(buf, testFileContents, testFileContentsLength);
    if (cmpResult != 0)
    {
        return false;
    }

    time_function_with_return(spPlatformDeleteFile(testFileName), success);
    if (success) return false;

    return true;;

    failure:
    spPlatformDeleteFile(testFileName);
    return false;

}

bool TestGLSLLoader(const char *restrict shaderPath)
{
    // TODO

    SpiritShader testShaderSource; // loaded from source
    time_function_with_return(spLoadSourceShader(
        shaderPath,
        SPIRIT_SHADER_TYPE_FRAGMENT), testShaderSource);

    if (testShaderSource.shaderSize == 0)
    {
        return false;
    }

    SpiritShader testShaderBinary; // loaded from binary
    time_function_with_return(spLoadSourceShader(
        shaderPath,
        SPIRIT_SHADER_TYPE_FRAGMENT), testShaderBinary);

    if (testShaderBinary.shaderSize == 0)
    {
        spDestroyShader(testShaderSource);
        return false;
    }

    bool equal = true;

    if (testShaderSource.shaderSize == testShaderBinary.shaderSize &&
        testShaderSource.type == testShaderBinary.type); else
    {
        log_warning("Shaders were not equal");
        equal = false;
    }


    if (equal && memcmp(
        testShaderSource.shader,
        testShaderBinary.shader,
        testShaderSource.shaderSize) != 0)
    {
        equal = false;
    }

    // formulate test shader binary name (hacked)
    u32 bufLen = 256;
    char stripBuf[256];
    spStringStrip(stripBuf, &bufLen, shaderPath, '/');

    char buf[256];
    npf_snprintf(buf, bufLen, "%s%s.spv", GLSL_LOADER_CACHE_FOLDER, stripBuf);

    spPlatformDeleteFile(buf);

    time_function(spDestroyShader(testShaderSource));
    time_function(spDestroyShader(testShaderBinary));

    return equal;
}


void Test(void)
{
    init_timer();
    for (size_t i = 0; i < 1; i++)
    {
        runTest(TestGLSLLoader("tests/test_shader.frag"));
        runTest(TestPlatformLocalizeFilename("file/test.png", "./bin/file/test.png"));
        runTest(TestPlatformLocalizeFilename("file\\test.mov", "./bin/file/test.mov"));
        runTest(TestStringTruncate("filename.txt", '.', false, "filename"));
        runTest(TestStringStrip("filename.txt", '.', "txt"));
        runTest(TestFileUtilities("testfile.txt", "Testing test file\nNewline test"));
    }
    terminate_timer();
}
