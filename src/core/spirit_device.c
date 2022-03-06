#include "spirit_device.h"

// Create and manage a rendering device rendering device
//
//
// Kael Johnston Feb 27 2022

#define log_verbose(messege, ...) if(createInfo.verbose)LOG_INFO(messege, ##__VA_ARGS__);

//
// Structures
//

//
// Helper Functions
//
static VkInstance createInstance (SpiritDeviceCreateInfo createInfo);

static VkBool32 debugCallback (
    VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
    void*                                            pUserData);



//
// Public Functions
//

SpiritDevice spCreateDevice (SpiritDeviceCreateInfo createInfo) {

    SpiritDevice out = new_var(struct t_SpiritDevice);

    out->deviceExtensionCount = createInfo.windowExtensions.count;
    out->deviceExtensions = createInfo.windowExtensions.names;

    out->instance = createInstance(createInfo);

    return out;
}

//
// Helper Implementation
//

static VkInstance createInstance (const SpiritDeviceCreateInfo createInfo) {

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = createInfo.appName;
    appInfo.applicationVersion = createInfo.appVersion;
    appInfo.pEngineName = createInfo.engineName;
    appInfo.engineVersion = createInfo.engineVersion;
    appInfo.apiVersion = VK_API_VERSION_1_3;


    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    // extensions
    u32 extensionCount = 0;
    if (createInfo.enableValidation) extensionCount++; // if using validation, make room for valiation extensions
    const char **extensions = new_array(char*, extensionCount);
    for (u32 i = 0; i < createInfo.windowExtensions.count; i++) {
        extensions[i] = createInfo.windowExtensions.names[i];
    }
    if (createInfo.enableValidation) extensions[extensionCount-1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME; // add debug extension if it should
    instanceInfo.enabledExtensionCount = extensionCount;
    instanceInfo.ppEnabledExtensionNames = extensions;

    // validation layers
    if (createInfo.enableValidation) {
        log_verbose("Enabling Validation");
        log_verbose("Layer Count = %i", createInfo.requiredValidationLayerCount);
        // layer names and whatnot
        const char **validationLayers = (const char **) createInfo.requiredValidationLayers;
        instanceInfo.enabledLayerCount = createInfo.requiredValidationLayerCount;
        if (createInfo.requiredValidationLayerCount == 0) {
            instanceInfo.enabledLayerCount = 1;
            const char *tmpLayer = "VK_LAYER_KHRONOS_validation";
            validationLayers = &tmpLayer;
        }
        instanceInfo.ppEnabledLayerNames = validationLayers;

        // debug messenger
        VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugInfo.pfnUserCallback = debugCallback;

        instanceInfo.pNext = &debugInfo;

    } else {
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.ppEnabledLayerNames = NULL;
    }

    VkInstance instance;

    if (vkCreateInstance(&instanceInfo, NULL, &instance) != VK_SUCCESS) {
        return SPIRIT_NULL;
    }

    return instance;

}

static VkBool32 debugCallback (
    VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
    void*                                            pUserData) {
    
    if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOG_VALIDATION(pCallbackData->pMessage);
        return true;
    } else return false;
}
