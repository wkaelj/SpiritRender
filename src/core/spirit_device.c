#include "spirit_device.h"

// Create and manage a rendering device rendering device
//
//
// Kael Johnston Feb 27 2022

//
// Structures
//

// queue info
// I defined them this way so more queues could be added in the future
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
static VkInstance createInstance (const SpiritDeviceCreateInfo *createInfo, VkDebugUtilsMessengerEXT *debugMessenger); // create a vulkan device
static VkPhysicalDevice selectPhysicalDevice (const SpiritDeviceCreateInfo *createInfo, const VkInstance instance); // select a gpu
static VkDevice createDevice (const SpiritDeviceCreateInfo *createInfo, const VkPhysicalDevice physicalDevice, const VkInstance instance);

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (
    VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
    void*                                            pUserData);


// find required device queues
static QueueFamilyIndices findDeviceQueues (const SpiritDeviceCreateInfo *createInfo, VkPhysicalDevice questionedDevice);
static SpiritSwapchainSupportInfo querySwapChainSupport (const SpiritDeviceCreateInfo *createInfo, VkPhysicalDevice questionedDevice);

//
// Public Functions
//

SpiritDevice spCreateDevice (SpiritDeviceCreateInfo *createInfo) {

    // asserts
    db_assert(createInfo->window, "Must have window to create instance");


    SpiritDevice out = new_var(struct t_SpiritDevice);

    out->deviceExtensionCount = createInfo->windowExtensions.count;
    out->deviceExtensions = createInfo->windowExtensions.names;

    // validation layers

    // default validation layers
    if (createInfo->enableValidation && createInfo->requiredValidationLayerCount == 0) {
        const char *backupLayer = "VK_LAYER_KHRONOS_validation";
        createInfo->requiredValidationLayers = &backupLayer;
        createInfo->requiredValidationLayerCount = 1;
    }
    // check for validation layer support, and automatically disable them
    // if they are not available
    if (createInfo->enableValidation &&
    !checkValidationLayerSupport(
        createInfo->requiredValidationLayers, 
        createInfo->requiredValidationLayerCount)) {
        
        // disable validation
        createInfo->enableValidation = SPIRIT_FALSE;
        log_validation ("Automatically disabling validation, because it is not supported by the GPU");
    }

    // fallback device extensions
    if (createInfo->requiredDeviceExtensionCount == 0) {
        const char *requiredDeviceExtensions[1] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        createInfo->requiredDeviceExtensionCount = 1;
        createInfo->requiredDeviceExtensions = requiredDeviceExtensions;
    }

    out->validationEnabled = createInfo->enableValidation;

    out->instance = createInstance(createInfo, &out->debugMessenger); // create vulkan instance
    if (out->instance == NULL) {
        log_fatal("Failed to create vulkan instance");
        return NULL;
    }
    out->windowSurface = spWindowGetSurface(createInfo->window, out->instance); // create window surface
    createInfo->windowSurface = out->windowSurface;
    out->physicalDevice = selectPhysicalDevice(createInfo, out->instance); // select physical device
    if (out->physicalDevice == NULL) {
        log_fatal("Failed to select GPU");
        return NULL;
    }

    out->swapchainDetails = querySwapChainSupport(createInfo, out->physicalDevice);
    out->device = createDevice(createInfo, out->physicalDevice, out->instance);
    if (out->device == NULL) {
        log_fatal("Failed to create logical device");
        return NULL;
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

    vkDestroyDevice(device->device, NULL);

    vkDestroySurfaceKHR(device->instance, device->windowSurface, NULL);

    // debug messenger
    if (device->validationEnabled) {
        PFN_vkDestroyDebugUtilsMessengerEXT pfnDebugMessengerDestroy = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(device->instance, "vkDestroyDebugUtilsMessengerEXT");
        if (pfnDebugMessengerDestroy == NULL) log_error("Failed to load debug messenger destroy function");
        else pfnDebugMessengerDestroy (device->instance, device->debugMessenger, NULL);
        log_debug("Destroyed debug messenger");
    }
    device->debugMessenger = NULL;

    vkDestroyInstance(device->instance, NULL);
    dalloc(device);

    return SPIRIT_SUCCESS;
}

//
// Private Helpers
//

// check if physical device is suitable for vulkan device
static u16 isDeviceSuitable (const SpiritDeviceCreateInfo *createInfo, VkPhysicalDevice questionedDevice);

// check available GPU extensions
static SpiritBool checkDeviceExtensionSupport (const SpiritDeviceCreateInfo *createInfo, VkPhysicalDevice questionedDevice);



//
// Helper Implementation
//

static VkInstance createInstance (const SpiritDeviceCreateInfo *createInfo, VkDebugUtilsMessengerEXT *debugMessenger) {



    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = createInfo->appName;
    appInfo.applicationVersion = createInfo->appVersion;
    appInfo.pEngineName = createInfo->engineName;
    appInfo.engineVersion = createInfo->engineVersion;
    appInfo.apiVersion = VK_API_VERSION_1_3;


    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    // extensions
    u32 extensionCount = createInfo->windowExtensions.count;
    if (createInfo->enableValidation) extensionCount += 1; // if using validation, make room for valiation extensions
    const char **extensions = new_array(const char*, extensionCount);
    for (u32 i = 0; i < createInfo->windowExtensions.count && i < extensionCount; i++) {
        extensions[i] = createInfo->windowExtensions.names[i];
    }
    if (createInfo->enableValidation) extensions[extensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME; // add debug extension if it should
    instanceInfo.enabledExtensionCount = extensionCount;
    instanceInfo.ppEnabledExtensionNames = extensions;

    // validation layers
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {}; // messenger create info
    const char *const *validationLayers = NULL;
    instanceInfo.enabledLayerCount = 0;
    if (createInfo->enableValidation) {
        log_verbose("Enabling Validation");

        // allocate memory for the debugmessenger if it is NULL
        db_assert(debugMessenger, "debugMessenger was NULL, it should be a \
            reference to the &SpiritDevice.debugMessenger variable");
        // layer names and whatnot
        instanceInfo.enabledLayerCount = createInfo->requiredValidationLayerCount;
        validationLayers = createInfo->requiredValidationLayers;
        instanceInfo.enabledLayerCount = createInfo->requiredValidationLayerCount;
        instanceInfo.ppEnabledLayerNames = validationLayers;

        // debug messenger
        debugInfo.sType = 
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity =             
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugInfo.messageType = 
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugInfo.pfnUserCallback = debugCallback;

        instanceInfo.pNext = &debugInfo;

    } else {
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.ppEnabledLayerNames = NULL;
    }

    VkInstance instance;

    if (vkCreateInstance(&instanceInfo, NULL, &instance) != VK_SUCCESS) {
        return NULL;
    }

    if (createInfo->enableValidation) {
        PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugMesseger = NULL;
        pfnCreateDebugMesseger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

        if (pfnCreateDebugMesseger == NULL) log_error("Failed to load function vkCreateDebugUtilsMessengerEXT. Will log errors to stdout.");
        else {
            if (pfnCreateDebugMesseger (instance, &debugInfo, NULL, debugMessenger) != VK_SUCCESS) log_error("Failed to create debug messenger");
            *debugMessenger = NULL;
        }
    }

    log_verbose("Created Vulkan Instance");
    return instance;

}

static VkPhysicalDevice selectPhysicalDevice (const SpiritDeviceCreateInfo *createInfo, const VkInstance instance) {

    // set device to null so i can mess with it
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    // check available rendering devices
    uint32_t availableDeviceCount = 0;
    vkEnumeratePhysicalDevices (instance, &availableDeviceCount, NULL);
    //throw error if there are no devices
    if (availableDeviceCount == 0) {
        log_fatal("No GPU detected.");
        return NULL;
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
        log_fatal("No suitable GPU detected.");
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

    // log selected device name
#ifdef ENABLE_VERBOSE
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties (
            outputDevice.physicalDevice,
            &properties);
        log_info("Selected GPU '%s'", properties.deviceName);
#endif
    // set device to highest ranking device
    return outputDevice.physicalDevice;

}

static VkDevice createDevice (const SpiritDeviceCreateInfo *createInfo, const VkPhysicalDevice physicalDevice, const VkInstance instance) {

    QueueFamilyIndices indices = findDeviceQueues (createInfo, physicalDevice);

    // create queues
    f32 queuePriority = 1.0f; // float to be referenced (&queuePriority)
    u32 queueCount = QUEUE_COUNT;
    u32 queueFamilies[QUEUE_COUNT] = QUEUE_NAMES(indices);
    VkDeviceQueueCreateInfo queueCreateInfos[queueCount];
    u32 addedQueues[QUEUE_COUNT];
    u32 skippedQueueCount = 0;    
    for (u32 i = 0; i < queueCount; i++) {
        bool isDuplicateQueue = false;
        // avoid adding duplicate queues
        // the < i is to ensure that we don't check indexes of
        // addedQueues that have not been set yet (false positives with 0)
        for (u32 n = 0; n < queueCount && n < i; n++) {
            if (queueFamilies[i] == addedQueues[n]) {
                isDuplicateQueue = true;
            }
        }
        if (isDuplicateQueue) {
            log_verbose("Skipping duplicate queue index: %u",
                queueFamilies[i]);
            skippedQueueCount++;
            continue;
        }
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilies[i];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        // subtract skipped queues to avoid blank spaces
        // [set, set, duplicate, set] -> [set, set, set]
        queueCreateInfos[i - skippedQueueCount] = queueCreateInfo;
        addedQueues[i - skippedQueueCount] = queueFamilies[i];
    }

    VkPhysicalDeviceFeatures deviceFeatures = {}; // populate later

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = queueCount - skippedQueueCount;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    // enable device extensions
    deviceCreateInfo.enabledExtensionCount = createInfo->requiredDeviceExtensionCount;
    deviceCreateInfo.ppEnabledExtensionNames = createInfo->requiredDeviceExtensions;

    // pass used enabled validation layers
    deviceCreateInfo.enabledLayerCount = createInfo->requiredValidationLayerCount;
    deviceCreateInfo.ppEnabledLayerNames = createInfo->requiredValidationLayers;

    VkDevice device;
    VkResult errCode;
    if (errCode = vkCreateDevice (physicalDevice, &deviceCreateInfo, NULL, &device) != VK_SUCCESS) {
        // debug function
        log_info("Device failure code '%d'", errCode);
        return NULL;
    }

    log_verbose("Created logical device");
    return device;
}

// debug callback function
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (
    VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT      *pCallbackData,
    void                                            *pUserData) {
    
    if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        log_validation("%s", pCallbackData->pMessage);
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
        for (u32 x = 0; x < layerCount; x++) {
            if (strcmp (requiredLayerNames[i], availableLayers[x].layerName) == 0) {
                log_debug("Found layer: %s", availableLayers[x].layerName);
                found = true;
                break;
            }
        }

        if (found == false) {
            log_debug("Unavailable valiation layer: %s", requiredLayerNames[i]);
            return SPIRIT_FALSE;
        }
    }

    return SPIRIT_TRUE;
}

// Physical device

// function to check if a physical device meets requirements
static u16 isDeviceSuitable (const SpiritDeviceCreateInfo *createInfo, VkPhysicalDevice questionedDevice) {

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
        log_error("Device '%s' lacks queue support", deviceProperties.deviceName);
    if (!hasExtensionSupport)
        log_error("Device '%s' lacks extension support", deviceProperties.deviceName);
    if (!supportsSwapchain)
        log_error("Device '%s' lacks swapchain support", deviceProperties.deviceName);

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
        if (createInfo->powerSaveMode) {
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
static SpiritBool checkDeviceExtensionSupport (const SpiritDeviceCreateInfo *createInfo, VkPhysicalDevice questionedDevice) {

    u32 availailableExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties (questionedDevice, NULL, &availailableExtensionCount, NULL);
    VkExtensionProperties availableExtensionNames[availailableExtensionCount];
    vkEnumerateDeviceExtensionProperties (questionedDevice, NULL, &availailableExtensionCount, availableExtensionNames);
    // highly efficient iterator (two means twice the speed)
    for (u32 i = 0; i < createInfo->requiredDeviceExtensionCount; i++) {
        bool found = false; // if the current extension has been found
        for (uint32_t x = 0; x < availailableExtensionCount; x++) {
            if (strcmp (createInfo->requiredDeviceExtensions[i], availableExtensionNames[x].extensionName) == 0) {
                log_verbose("Found required device extension '%s'", availableExtensionNames[x].extensionName);
                found = true;
                break;
            }
        }
        // "handle" missing layer
        if (!found) {
            log_fatal("GPU does not support required extension: %s", createInfo->requiredDeviceExtensions[i]);
            return SPIRIT_FALSE;
        }
    }

    return SPIRIT_TRUE;
}

// helper function to access required device queues
static QueueFamilyIndices findDeviceQueues (const SpiritDeviceCreateInfo *createInfo, VkPhysicalDevice questionedDevice) {

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
            if (vkGetPhysicalDeviceSurfaceSupportKHR (questionedDevice, i, createInfo->windowSurface, &presentationSupport) != VK_SUCCESS)
                log_info("Failed find presentation support!");
            
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
static SpiritSwapchainSupportInfo querySwapChainSupport (const SpiritDeviceCreateInfo *createInfo, VkPhysicalDevice questionedDevice) {
    SpiritSwapchainSupportInfo details = {};

    // check surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR (
        questionedDevice,
        createInfo->windowSurface, 
        &details.capabilties);

    // formats
    vkGetPhysicalDeviceSurfaceFormatsKHR (
        questionedDevice, 
        createInfo->windowSurface, 
        &details.formatCount, NULL);
    db_assert(details.formatCount, "Device surface has no format.");
    details.formats = malloc (sizeof (VkSurfaceFormatKHR) * details.formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR (questionedDevice, createInfo->windowSurface, &details.formatCount, details.formats);
    db_assert (details.formats, "Device surface has no formats");

    if (details.formatCount == 0) { details.formats = NULL; }

    // present modes
    vkGetPhysicalDeviceSurfacePresentModesKHR (questionedDevice, createInfo->windowSurface, &details.presentModeCount, NULL);
    db_assert (details.presentModeCount > 0,
        "Device surface has no present modes");
    details.presentModes = malloc (sizeof (VkSurfaceFormatKHR) * details.presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR (questionedDevice, createInfo->windowSurface, &details.presentModeCount, details.presentModes);
    db_assert (details.presentModes != NULL,
        "Device surface has no present modes");

    return details;
}
