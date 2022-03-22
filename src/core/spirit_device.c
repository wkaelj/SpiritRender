#include "spirit_device.h"

// Create and manage a rendering device rendering device
//
//
// Kael Johnston Feb 27 2022

//
// Structures
//

// queue info

#define QUEUE_COUNT 2
#define QUEUE_NAMES(varname) {varname.graphicsQueue, varname.presentQueue}


// structure to aid in queue family selection
typedef struct t_QueueFamilyIndices {
    
    u32 graphicsQueue;
    SpiritBool foundGraphicsQueue;
    u32 presentQueue;
    SpiritBool foundPresentQueue;
} QueueFamilyIndices;

// structure to manage rendering device candidates
typedef struct {
    VkPhysicalDevice physicalDevice;
    uint16_t deviceScore;
} SuitableDevice;

//
// Helper Functions
//

// create a vulkan instance
static SpiritBool checkValidationLayerSupport (const char *const *requiredLayerNames, u32 requiredLayerCount);
static VkInstance createInstance (SpiritDeviceCreateInfo createInfo, VkDebugUtilsMessengerEXT *debugMessenger); // create a vulkan device
static VkPhysicalDevice selectPhysicalDevice (SpiritDeviceCreateInfo createInfo, const VkInstance instance); // select a gpu
static VkDevice createDevice (SpiritDeviceCreateInfo createInfo, const VkPhysicalDevice physicalDevice, const VkInstance instance);

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (
    VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
    void*                                            pUserData);


// find required device queues
static QueueFamilyIndices findDeviceQueues (SpiritDeviceCreateInfo createInfo, VkPhysicalDevice questionedDevice);
static SpiritSwapchainSupportInfo querySwapChainSupport (SpiritDeviceCreateInfo createInfo, VkPhysicalDevice questionedDevice);

//
// Public Functions
//

SpiritDevice spCreateDevice (SpiritDeviceCreateInfo createInfo) {

    SpiritDevice out = new_var(struct t_SpiritDevice);

    out->deviceExtensionCount = createInfo.windowExtensions.count;
    out->deviceExtensions = createInfo.windowExtensions.names;

    // validation layers
    // default validation layers
    if (createInfo.enableValidation && createInfo.requiredValidationLayerCount == 0) {

        const char *backupLayer = "VK_LAYER_KHRONOS_validation";
        createInfo.requiredValidationLayers = &backupLayer;
        createInfo.requiredValidationLayerCount = 1;
    }
    if (createInfo.enableValidation && !checkValidationLayerSupport(createInfo.requiredValidationLayers, createInfo.requiredValidationLayerCount)) {
        createInfo.enableValidation = SPIRIT_FALSE;
    }

    // fallback device extensions
    if (createInfo.requiredDeviceExtensionCount == 0) {
        const char *requiredDeviceExtensions[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        createInfo.requiredDeviceExtensionCount = 1;
        createInfo.requiredDeviceExtensions = requiredDeviceExtensions;
    }

    out->validationEnabled = createInfo.enableValidation;

    out->instance = createInstance(createInfo, &out->debugMessenger); // create vulkan instance
    if (out->instance == SPIRIT_NULL) {
        LOG_FATAL("Failed to create vulkan instance");
        return SPIRIT_NULL;
    }
    out->windowSurface = spWindowGetSurface(createInfo.window, out->instance); // create window surface
    createInfo.windowSurface = out->windowSurface;
    out->physicalDevice = selectPhysicalDevice(createInfo, out->instance); // select physical device
    if (out->physicalDevice == SPIRIT_NULL) {
        LOG_FATAL("Failed to select GPU");
        return SPIRIT_NULL;
    }

    out->swapchainDetails = querySwapChainSupport(createInfo, out->physicalDevice);
    out->device = createDevice(createInfo, out->physicalDevice, out->instance);
    if (out->device == SPIRIT_NULL) {
        LOG_FATAL("Failed to create logical device");
        return SPIRIT_NULL;
    }

    QueueFamilyIndices indices = findDeviceQueues (createInfo, out->physicalDevice);
    u32 queues[QUEUE_COUNT] = QUEUE_NAMES(indices);
    out->queueIndices = queues;
    out->queueCount = QUEUE_COUNT;
    vkGetDeviceQueue (out->device, indices.graphicsQueue, 0, &out->graphicsQueue); // create graphics queue
    vkGetDeviceQueue (out->device, indices.presentQueue, 0, &out->presentQueue); // create present queue

    return out;
}

// destroy a spirit device and free all memory whatever
SpiritResult spDestroyDevice (SpiritDevice device) {

    vkDestroyDevice(device->device, SPIRIT_NULL);

    vkDestroySurfaceKHR(device->instance, device->windowSurface, SPIRIT_NULL);
    // debug messenger
    if (device->validationEnabled) {
        PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(device->instance, "vkDestroyDebugUtilsMessengerExt");
        if (func == SPIRIT_NULL) LOG_ERROR("Failed to load debug messenger destroy function");
        else func (device->instance, device->debugMessenger, SPIRIT_NULL);
    }

    vkDestroyInstance(device->instance, SPIRIT_NULL);

    spMemFree(device);

    return SPIRIT_SUCCESS;
}

//
// Private Helpers
//

// check if physical device is suitable for vulkan device
static u16 isDeviceSuitable (SpiritDeviceCreateInfo createInfo, VkPhysicalDevice questionedDevice);

// check available GPU extensions
static SpiritBool checkDeviceExtensionSupport (SpiritDeviceCreateInfo createInfo, VkPhysicalDevice questionedDevice);



//
// Helper Implementation
//

static VkInstance createInstance (const SpiritDeviceCreateInfo createInfo, VkDebugUtilsMessengerEXT *debugMessenger) {

    if (createInfo.enableValidation && debugMessenger == NULL) debugMessenger = new_var (VkDebugUtilsMessengerEXT);

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
    u32 extensionCount = createInfo.windowExtensions.count;
    if (createInfo.enableValidation) extensionCount += 1; // if using validation, make room for valiation extensions
    const char **extensions = new_array(char*, extensionCount);
    for (u32 i = 0; i < createInfo.windowExtensions.count && i < extensionCount; i++) {
        extensions[i] = createInfo.windowExtensions.names[i];
    }
    if (createInfo.enableValidation) extensions[extensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME; // add debug extension if it should
    instanceInfo.enabledExtensionCount = extensionCount;
    instanceInfo.ppEnabledExtensionNames = extensions;

    // validation layers
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {}; // messenger create info
    const char *const *validationLayers = SPIRIT_NULL;
    instanceInfo.enabledLayerCount = 0;
    if (createInfo.enableValidation) {
        log_verbose("Enabling Validation");
        // layer names and whatnot
        instanceInfo.enabledLayerCount = createInfo.requiredValidationLayerCount;
        validationLayers = createInfo.requiredValidationLayers;
        instanceInfo.enabledLayerCount = createInfo.requiredValidationLayerCount;
        instanceInfo.ppEnabledLayerNames = validationLayers;

        // debug messenger
        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugInfo.pfnUserCallback = debugCallback;

        instanceInfo.pNext = &debugInfo;

    } else {
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.ppEnabledLayerNames = SPIRIT_NULL;
    }

    VkInstance instance;

    if (vkCreateInstance(&instanceInfo, SPIRIT_NULL, &instance) != VK_SUCCESS) {
        return SPIRIT_NULL;
    }

    if (createInfo.enableValidation) {
        PFN_vkCreateDebugUtilsMessengerEXT func = SPIRIT_NULL;
        func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if (func == SPIRIT_NULL) LOG_ERROR("Failed to load function vkCreateDebugUtilsMessengerEXT. Will log errors to stdout.");
        else {
            if (func (instance, &debugInfo, SPIRIT_NULL, debugMessenger) != VK_SUCCESS) LOG_ERROR("Failed to create debug messenger");
            *debugMessenger = NULL;
        }
    }

    log_verbose("Created Vulkan Instance");
    return instance;

}

static VkPhysicalDevice selectPhysicalDevice (SpiritDeviceCreateInfo createInfo, const VkInstance instance) {

    // set device to null so i can mess with it
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    // check available rendering devices
    uint32_t availableDeviceCount = 0;
    vkEnumeratePhysicalDevices (instance, &availableDeviceCount, NULL);
    //throw error if there are no devices
    if (availableDeviceCount == 0) {
        LOG_FATAL("No GPU detected.");
        return SPIRIT_NULL;
    }

    VkPhysicalDevice availableDevices[availableDeviceCount];
    vkEnumeratePhysicalDevices (instance, &availableDeviceCount, availableDevices);

    uint32_t suitableDeviceCount = 0;
    SuitableDevice suitableDevices[availableDeviceCount];

    uint16_t tmpDeviceOutput = 0; // used to store function output in for loop
    for (uint32_t i = 0; i < availableDeviceCount; i++) {
        tmpDeviceOutput = isDeviceSuitable (createInfo, availableDevices[i]);
        if (tmpDeviceOutput > 0) {
            suitableDevices[suitableDeviceCount].physicalDevice = availableDevices[i];
            suitableDevices[suitableDeviceCount].deviceScore = tmpDeviceOutput;
            suitableDeviceCount++;
        }
    }

    // check if there are no suitable GPU
    if (suitableDeviceCount == 0) {
        LOG_FATAL("No suitable GPU detected.");
        return NULL;
    }

    // select highest ranking device
    SuitableDevice outputDevice;
    outputDevice.deviceScore = 0;
    outputDevice.physicalDevice = VK_NULL_HANDLE;
    for (uint16_t i = 0; i < suitableDeviceCount; i++) {

        if (suitableDevices[i].deviceScore > outputDevice.deviceScore) {
            outputDevice.deviceScore = suitableDevices[i].deviceScore;
            outputDevice.physicalDevice = suitableDevices[i].physicalDevice;
        }
    }

#ifdef ENABLE_VERBOSE
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties (outputDevice.physicalDevice, &properties);
        LOG_INFO("Selected GPU '%s'", properties.deviceName);
#endif
    // set device to highest ranking device
    return outputDevice.physicalDevice;

}

static VkDevice createDevice (SpiritDeviceCreateInfo createInfo, const VkPhysicalDevice physicalDevice, const VkInstance instance) {

    QueueFamilyIndices indices = findDeviceQueues (createInfo, physicalDevice);

    // create queues
    float queuePriority = 1.0f; // float pointer for later
    uint32_t queueCount = QUEUE_COUNT; // queue count
    uint32_t queueFamilies[QUEUE_COUNT] = QUEUE_NAMES(indices); // queue indices
    VkDeviceQueueCreateInfo queueCreateInfos[queueCount]; // queue create infos

    for (uint32_t i = 0; i < queueCount; i++) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilies[i];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos[i] = queueCreateInfo;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {}; // populate later

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = queueCount;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    // enable device extensions
    deviceCreateInfo.enabledExtensionCount = createInfo.requiredDeviceExtensionCount;
    deviceCreateInfo.ppEnabledExtensionNames = createInfo.requiredDeviceExtensions;

    // pass used enabled validation layers
    deviceCreateInfo.enabledLayerCount = createInfo.requiredValidationLayerCount;
    deviceCreateInfo.ppEnabledLayerNames = createInfo.requiredValidationLayers;

    VkDevice device;
    if (vkCreateDevice (physicalDevice, &deviceCreateInfo, NULL, &device) != VK_SUCCESS) {
        // debug function
        return SPIRIT_NULL;
    }

    log_verbose("Created logical device");
    return device;
}

// debug callback function
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (
    VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
    void*                                            pUserData) {
    
    if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOG_VALIDATION("%s", pCallbackData->pMessage);
        return true;
    } else return false;
}

//
// Private Helpers
//

// instance
static SpiritBool checkValidationLayerSupport (const char *const *requiredLayerNames, u32 requiredLayerCount) {
    u32 layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties availableLayers[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (u32 i = 0; i < requiredLayerCount; i++)
    {
        bool found = false;

        //iterate through available extensions
        for (uint32_t x = 0; x < layerCount; x++) {
            if (spStringCmp (requiredLayerNames[i], availableLayers[x].layerName, 0) == SPIRIT_TRUE) {
                LOG_DEBUG("Found layer: %s", availableLayers[x].layerName);
                found = true;
                break;
            }
        }

        if (found == false) {
            LOG_DEBUG("Unavailable valiation layer: %s", requiredLayerNames[i]);
            return SPIRIT_TRUE;
        }
    }

    return SPIRIT_FALSE;
}

// Physical device

// function to check if a physical device meets requirements
static u16 isDeviceSuitable (SpiritDeviceCreateInfo createInfo, VkPhysicalDevice questionedDevice) {

    // get random device info
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties (questionedDevice, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures (questionedDevice, &deviceFeatures);
    
    uint16_t score = 0; // the device priority when selecting in future

    // requirment bools
    bool hasExtensionSupport = false; // if device support required extensions
    bool supportsQueues = false; // if device supports required queues
    bool supportsSwapchain = false; // if device supports adequate swapchain functionality

    // check if device supports required extensions
    if (checkDeviceExtensionSupport (createInfo, questionedDevice) == SPIRIT_TRUE) { hasExtensionSupport = true; }

    // check if device support graphics queue and presentation support
    QueueFamilyIndices deviceQueue = {};
    deviceQueue = findDeviceQueues (createInfo, questionedDevice);
    if ( deviceQueue.foundGraphicsQueue && deviceQueue.foundPresentQueue) { supportsQueues = true; }

    // check if device has swapchain support
    SpiritSwapchainSupportInfo swapchainDetails = querySwapChainSupport (createInfo, questionedDevice);
    supportsSwapchain = swapchainDetails.formatCount > 0 && swapchainDetails.presentModeCount > 0;


    // DEBUG
    if (!supportsQueues)
        LOG_ERROR("Device '%s' lacks queue support", deviceProperties.deviceName);
    if (!hasExtensionSupport)
        LOG_ERROR("Device '%s' lacks extension support", deviceProperties.deviceName);
    if (!supportsSwapchain)
        LOG_ERROR("Device '%s' lacks swapchain support", deviceProperties.deviceName);

    // check if device supports all functionality
    if (hasExtensionSupport && supportsQueues && supportsSwapchain) {
        score = 1;
    }

    // score device based on type of gpu
    if (score == 0) {
        log_verbose("Device '%s' has score %i", deviceProperties.deviceName, score);
        return 0;

    } else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 4;
    } else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        // increase priority for integrated GPU if wanted
        if (createInfo.powerSaveMode) {
            score += 3;
        }
        score += 2;
    } else {
        score = 0;
    }

    //increase gpu score if same queue supports everything
    if (deviceQueue.graphicsQueue == deviceQueue.presentQueue) {
        score += 1;
    }
    return score;
}

// function to check if a physical device supports swapchain
static SpiritBool checkDeviceExtensionSupport (SpiritDeviceCreateInfo createInfo, VkPhysicalDevice questionedDevice) {

    u32 availailableExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties (questionedDevice, NULL, &availailableExtensionCount, NULL);
    VkExtensionProperties availableExtensionNames[availailableExtensionCount];
    vkEnumerateDeviceExtensionProperties (questionedDevice, NULL, &availailableExtensionCount, availableExtensionNames);
    // highly efficient iterator (two means twice the speed)
    for (u32 i = 0; i < createInfo.requiredDeviceExtensionCount; i++) {
        bool found = false; // if the current extension has been found
        for (uint32_t x = 0; x < availailableExtensionCount; x++) {
            if (spStringCmp (createInfo.requiredDeviceExtensions[i], availableExtensionNames[x].extensionName, 0) == SPIRIT_TRUE) {
                log_verbose("Found required device extension '%s'", availableExtensionNames[x].extensionName);
                found = true;
                break;
            }
        }
        // "handle" missing layer
        if (!found) {
            LOG_FATAL("GPU does not support required extension: %s", createInfo.requiredDeviceExtensions[i]);
            return SPIRIT_FALSE;
        }
    }

    return SPIRIT_TRUE;
}

// helper function to access required device queues
static QueueFamilyIndices findDeviceQueues (SpiritDeviceCreateInfo createInfo, VkPhysicalDevice questionedDevice) {

    QueueFamilyIndices indices = {};

    // get device queue properties
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties (questionedDevice, &queueFamilyCount, NULL);
    VkQueueFamilyProperties deviceQueueProperties[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties (questionedDevice, &queueFamilyCount, deviceQueueProperties);

    // check for desired queue
    VkBool32 presentationSupport = false;
    for (uint32_t i = 0; i < queueFamilyCount; i++) {

        // check for graphics queue support if not already found
        if (!indices.foundGraphicsQueue) {
            if (deviceQueueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsQueue = i;
                indices.foundGraphicsQueue = true;

            }
        }

        // check for presentationSupport if not already found
        if (!indices.foundPresentQueue) {
            if (vkGetPhysicalDeviceSurfaceSupportKHR (questionedDevice, i, createInfo.windowSurface, &presentationSupport) != VK_SUCCESS)
                LOG_INFO("Failed find presentation support!");
            
            if (presentationSupport) {
                assert (presentationSupport);
                indices.foundPresentQueue = presentationSupport;
                indices.presentQueue = i;
            }
        }

        // check if all queues found
        if (indices.foundPresentQueue && indices.foundGraphicsQueue) {
            break;
        }
    }

    return indices;
}

// function to check swapchain support
static SpiritSwapchainSupportInfo querySwapChainSupport (SpiritDeviceCreateInfo createInfo, VkPhysicalDevice questionedDevice) {
    SpiritSwapchainSupportInfo details = {};

    // check surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR (questionedDevice, createInfo.windowSurface, &details.capabilties);

    // formats
    vkGetPhysicalDeviceSurfaceFormatsKHR (questionedDevice, createInfo.windowSurface, &details.formatCount, NULL);
    assert (details.formatCount > 0);
    details.formats = malloc (sizeof (VkSurfaceFormatKHR) * details.formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR (questionedDevice, createInfo.windowSurface, &details.formatCount, details.formats);
    assert (details.formats != NULL);

    if (details.formatCount == 0) { details.formats = NULL; }

    // present modes
    vkGetPhysicalDeviceSurfacePresentModesKHR (questionedDevice, createInfo.windowSurface, &details.presentModeCount, NULL);
    assert (details.presentModeCount > 0);
    details.presentModes = malloc (sizeof (VkSurfaceFormatKHR) * details.presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR (questionedDevice, createInfo.windowSurface, &details.presentModeCount, details.presentModes);
    assert (details.presentModes != NULL);

    return details;
}
