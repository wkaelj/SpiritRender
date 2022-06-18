// test main function

#include <spirit_header.h>

// types
#include "core/spirit_context.h"

// utils
#include "glsl-loader/glsl_loader.h"
#include "utils/spirit_file.h"
#include "utils/platform.h"

void mainlooptest (void);

int main (int argc, char **argv) {

    spPlatformSetExecutableFolder (argv[0]);

    const char *path = "shaders/simple_shader.frag";
    u64 srcLen = spReadFileSize(path);
    log_debug("Src length = %u", srcLen);
    char src[srcLen + 1];
    spReadFileText(src, path, &srcLen);
    src[srcLen] = '\0';
    db_assert(srcLen == strlen(src), "");

    SpiritShader o = spCompileShader (src, srcLen, "frag", SPIRIT_SHADER_TYPE_FRAGMENT);

    db_assert(o.shader, "Shader failed");

    spDestroyShader(o);

    mainlooptest ();

    return 0;
}

void mainlooptest (void) {

    SpiritContextCreateInfo contextInfo = {};
    contextInfo.enableValidation = true;
    contextInfo.fragmentShader = "shaders/simple_shader.frag";
    contextInfo.vertexShader = "shader/simple_shader.vert";
    contextInfo.windowName = "Hi Triangle :D";
    contextInfo.windowSize = (SpiritResolution) {600, 600};
    contextInfo.windowFullscreen = false;

    SpiritContext context = spCreateContext(&contextInfo);

    SpiritMaterialCreateInfo materialInfo = {};
    materialInfo.name = "std";
    materialInfo.fragmentShader = "shaders/simple_shader.frag";
    materialInfo.vertexShader = "shaders/simple_shader.vert";

    SpiritMaterial material = spCreateMaterial(
        context,
        &materialInfo);
    context->materials = &material;
    context->materialCount = 1;

    while (!spWindowShouldClose (context->window)){

        spContextSubmitFrame(context);
        vkDeviceWaitIdle(context->device->device);
    }

    spDestroyContext(context);


}
