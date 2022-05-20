// test main function

#include <spirit_header.h>
#include "core/spirit_window.h"
#include "core/spirit_device.h"
#include "core/spirit_swapchain.h"
#include "glsl-loader/glsl_loader.h"
#include "utils/spirit_file.h"
#include "utils/platform.h"

void mainlooptest (void);

int main (int argc, char **argv) {

    spPlatformSetExecutableFolder (argv[0]);

    // mainlooptest ();

    //log_info ("Local directory = '%s'", spPlatformGetExecutableFolder ());

    char *path = "shaders/vert_shader.vert";
    u32 length = 31;

    u32 finalLength = 0;
    spPlatformLocalizeFileName (NULL, path, &finalLength);
    char finalStr[finalLength];
    spPlatformLocalizeFileName (finalStr, path, &finalLength);

    if (spWriteFileFolder ("testfolder/folder/hello_world"))
    {
        log_debug ("Failed to create file");
    }

    log_debug("Final String = %s", finalStr);

    return 0;

    //spWriteFileFolder("testfolder");

    SpiritShader vert, frag;
    frag = loadSourceShader (
        "shaders/simple_shader.frag", 
        SPIRIT_SHADER_TYPE_FRAGMENT);
    vert = loadSourceShader (
        "shaders/simple_shader.vert",
        SPIRIT_SHADER_TYPE_VERTEX);

    return 0;
}

void mainlooptest (void) {

    // create window
    SpiritWindowCreateInfo windowCreateInfo;
    windowCreateInfo.w = 800;
    windowCreateInfo.h = 600;
    windowCreateInfo.title = "Hello Spirit";
    windowCreateInfo.fullscreen = SPIRIT_FALSE;

    SpiritWindow window = spCreateWindow (&windowCreateInfo);
    db_assert(window, "Must have window");

    // create device
    SpiritDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.enableValidation = SPIRIT_TRUE;
    deviceCreateInfo.powerSaveMode = SPIRIT_FALSE;

    deviceCreateInfo.appName = "TestApp";
    deviceCreateInfo.appVersion = VK_MAKE_VERSION(0, 0, 0);
    deviceCreateInfo.engineName = "Spirit Render";
    deviceCreateInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);

    deviceCreateInfo.windowExtensions = spWindowGetExtensions(window);
    deviceCreateInfo.window = window;

    const char *deviceExtensions[1] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    deviceCreateInfo.requiredDeviceExtensions = deviceExtensions;
    deviceCreateInfo.requiredDeviceExtensionCount = 1;
    SpiritDevice device = spCreateDevice (&deviceCreateInfo);
    db_assert(device, "Must have device");

    // create swapchain
    SpiritSwapchainCreateInfo swapCreateInfo = {};
    spWindowGetPixelSize(window, &swapCreateInfo.windowWidthPx, &swapCreateInfo.windowHeightPx);

    SpiritSwapchain swapchain = spCreateSwapchain(swapCreateInfo, device, SPIRIT_NULL);
    db_assert(swapchain, "Must have swapchain");

    while (!spWindowShouldClose (window));

    spDestroySwapchain(swapchain, device);
    spDestroyDevice(device);
    spDestroyWindow (window);
}