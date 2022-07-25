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

    spPlatformSetExecutableFolder (argv[0]);

    // tests
    if (argc >= 2 && strcmp("--test", argv[1]) == 0)
    {
        Test();
        return 0;
    }

    mainlooptest();

    return 0;
}

void mainlooptest (void) {

    SpiritContextCreateInfo contextInfo = {};
    contextInfo.enableValidation = true;
    contextInfo.windowName = "Hi Triangle :D";
    contextInfo.windowSize = (SpiritResolution) {600, 600};
    contextInfo.windowFullscreen = false;

    SpiritContext context = spCreateContext(&contextInfo);

    if (context == NULL)
        return;

    SpiritMaterialCreateInfo materialInfo = {};
    materialInfo.name = "std";
    materialInfo.fragmentShader = "shaders/simple_shader.frag";
    materialInfo.vertexShader = "shaders/simple_shader.vert";

    SpiritMaterial material = spCreateMaterial(
        context,
        &materialInfo);
    if (material == NULL)
        return;

    context->materials = &material;
    context->materialCount = 1;

    vec3 meshVerts[] = {
        {0.0f, -0.5f, 0.0f},
        {0.5f, 0.5f, 0.0f},
        {-0.5f, 0.5f, 0.0f}
    };

    SpiritMeshCreateInfo meshInfo = {};
    meshInfo.vertCount = 3;
    meshInfo.verts = meshVerts;

    SpiritMesh mesh = spCreateMesh(context, &meshInfo);

    SpiritMeshManager meshManager = spCreateMeshManager(NULL);
    const SpiritMeshReference meshRef = spMeshManagerAddMesh(meshManager, mesh);

    while (!spWindowShouldClose (context->window))
    {
        if (spMaterialAddMesh(material, meshRef))
        {
            log_fatal("Failed to add mesh to material");
            exit(1);
        }

        if(spContextSubmitFrame(context))
        {
            log_fatal("Failed to submit frame");
            exit(1);
        }

        sleep(1);
    }

    vkDeviceWaitIdle(context->device->device);
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
