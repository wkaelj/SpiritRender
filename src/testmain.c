// test main function

#include <spirit_header.h>
#include <unistd.h>

// types
#include "core/spirit_context.h"

#include "core/spirit_mesh.h"
#include "core/spirit_material.h"

// utils
#include "glsl-loader/glsl_loader.h"
#include "utils/spirit_file.h"
#include "utils/platform.h"

void mainlooptest (void);

// run all the tests
bool Test(void);


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
    }

    // delete shaders


    mainlooptest();

    return 0;
}

void mainlooptest (void) {

    SpiritContextCreateInfo contextInfo = {};
    contextInfo.enableValidation = true;
    contextInfo.windowName = "Hi Square :D";
    contextInfo.windowSize = (SpiritResolution) {800, 600};
    contextInfo.windowFullscreen = false;

    SpiritContext context = spCreateContext(&contextInfo);

    log_debug("Context res %ux%u", 
        context->screenResolution.w, 
        context->screenResolution.w);

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

    SpiritWindowState windowState;
    while ((windowState = spWindowGetState(context->window)) != SPIRIT_WINDOW_CLOSED)
    {
        // handle window resized
        if (windowState == SPIRIT_WINDOW_RESIZED)
        {
            if(spContextHandleWindowResized(context)) continue;
        } else if (windowState == SPIRIT_WINDOW_RESIZING) continue;

        if (spMaterialAddMesh(material, meshRef))
        {
            log_error("Failed to add mesh to material");
            continue;
        }

        if(spContextSubmitFrame(context))
        {
            log_error("Failed to submit frame");
        }

        sleep(2);

    }

    vkDeviceWaitIdle(context->device->device);
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
    spPlatformLocalizeFileName(NULL, input, &max);
    char output[max];
    spPlatformLocalizeFileName(output, input, &max);
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

    spStringTruncate(dest, &len, input, slicer, inclusive);

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
    
    spStringStrip(dest, &len, input, slicer);

    if (strcmp(dest, expected))
    {
        log_error("Expected %s, got %s", expected, dest);
        return false;
    }
    return true;
}

bool Test(void)
{
    runTest(TestPlatformLocalizeFilename("file/test.png", "./bin/file/test.png"));
    runTest(TestPlatformLocalizeFilename("file\\test.mov", "./bin/file/test.mov"));
    runTest(TestStringTruncate("filename.txt", '.', false, "filename"));
    runTest(TestStringStrip("filename.txt", '.', "txt"));
}
