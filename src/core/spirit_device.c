#include "spirit_device.h"
#include "core/spirit_types.h"
#include "debug/messenger.h"
#include <sys/cdefs.h>

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
#define QUEUE_NAMES(varname)                        \
    {                                               \
        varname.graphicsQueue, varname.presentQueue \
    }

// structure to aid in queue family selection
typedef struct t_QueueFamilyIndices
{

    u32 graphicsQueue;
    bool foundGraphicsQueue;
    u32 presentQueue;
    bool foundPresentQueue;
} QueueFamilyIndices;

// structure to manage rendering device candidates
typedef struct
{
    VkPhysicalDevice physicalDevice;
    uint16_t deviceScore;
} SuitableDevice;

//
// Helper Functions
//

// create a vulkan instance
static bool checkValidationLayerSupport(
    const char *const *requiredLayerNames, u32 requiredLayerCount);

static VkInstance createInstance(
    const SpiritDeviceCreateInfo *createInfo,
    VkDebugUtilsMessengerEXT *debugMessenger); // create a vulkan device

static VkPhysicalDevice selectPhysicalDevice(
    const SpiritDeviceCreateInfo *createInfo,
    const VkInstance instance); // select a gpu

static VkDevice createDevice(
    const SpiritDeviceCreateInfo *createInfo,
    const VkPhysicalDevice physicalDevice);

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData);

// find required device queues
static QueueFamilyIndices findDeviceQueues(
    const SpiritDeviceCreateInfo *createInfo,
    VkPhysicalDevice questionedDevice);
static SpiritSwapchainSupportInfo querySwapChainSupport(
    const VkSurfaceKHR surface, VkPhysicalDevice questionedDevice);

static VkCommandPool
createCommandPool(VkDevice device, QueueFamilyIndices queueFamilies);

//
// Public Functions
//

SpiritDevice spCreateDevice(SpiritDeviceCreateInfo *createInfo)
{

    // asserts
    db_assert_msg(createInfo->window, "Must have window to create instance");

    SpiritDevice out = new_var(struct t_SpiritDevice);

    out->deviceExtensionCount = createInfo->windowExtensions.count;
    out->deviceExtensions     = createInfo->windowExtensions.names;


    // validation layers

    // default validation layers
    const char *backupLayer = "VK_LAYER_KHRONOS_validation";
    if (createInfo->enableValidation &&
        createInfo->requiredValidationLayerCount == 0)
    {
        createInfo->requiredValidationLayers     = &backupLayer;
        createInfo->requiredValidationLayerCount = 1;
    }
    // check for validation layer support, and automatically disable them
    // if they are not available
    if (createInfo->enableValidation &&
        !checkValidationLayerSupport(
            createInfo->requiredValidationLayers,
            createInfo->requiredValidationLayerCount))
    {

        // disable validation
        createInfo->enableValidation = false;
        log_validation("Automatically disabling validation, because it is not "
                       "supported by the GPU");
    }

    // fallback device extensions
    if (createInfo->requiredDeviceExtensionCount == 0)
    {
        const char *requiredDeviceExtensions[1] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        createInfo->requiredDeviceExtensionCount = 1;
        createInfo->requiredDeviceExtensions     = requiredDeviceExtensions;
    }

    out->validationEnabled = createInfo->enableValidation;

    out->instance = createInstance(
        createInfo, &out->debugMessenger); // create vulkan instance
    if (out->instance == NULL)
    {
        log_fatal("Failed to create vulkan instance");
        return NULL;
    }

    // get the window surface
    out->windowSurface = spWindowGetSurface(
        createInfo->window, out->instance); // create window surface
    createInfo->windowSurface = out->windowSurface;

    out->physicalDevice = selectPhysicalDevice(
        createInfo, out->instance); // select physical device
    if (out->physicalDevice == NULL)
    {
        log_fatal("Failed to select GPU");
        return NULL;
    }
    out->swapchainDetails = (SpiritSwapchainSupportInfo){};
    spDeviceUpdateSwapchainSupport(out);
    out->device = createDevice(createInfo, out->physicalDevice);
    if (out->device == NULL)
    {
        log_fatal("Failed to create logical device");
        return NULL;
    }

    QueueFamilyIndices indices =
        findDeviceQueues(createInfo, out->physicalDevice);
    u32 queues[QUEUE_COUNT] = QUEUE_NAMES(indices);
    out->queueIndices       = queues;
    out->queueCount         = QUEUE_COUNT;
    vkGetDeviceQueue(
        out->device,
        indices.graphicsQueue,
        0,
        &out->graphicsQueue); // create graphics queue
    vkGetDeviceQueue(
        out->device,
        indices.presentQueue,
        0,
        &out->presentQueue); // create present queue

    // command pool
    out->commandPool = createCommandPool(out->device, indices);

    return out;
}

// find the format supported by the device
VkFormat spDeviceFindSupportedFormat(
    const SpiritDevice device,
    const VkFormat *candidates,
    const u32 candidateCount,
    const VkImageTiling tiling,
    const VkFormatFeatureFlags features)
{

    for (u32 i = 0; i < candidateCount; i++)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(
            device->physicalDevice, candidates[i], &props);
        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features)
        {
            return candidates[i];
        }
        else if (
            tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features)
        {
            return candidates[i];
        }
    }

    log_error("Could not find supported format");
    return VK_FORMAT_UNDEFINED;
}

u32 spDeviceFindMemoryType(
    const SpiritDevice device,
    const u32 typeFilter,
    VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device->physicalDevice, &memProperties);
    for (u32 i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) ==
                properties)
        {
            return i;
        }
    }
    log_fatal("Could not find memory typpe");
    abort();
    return -1;
}

SpiritResult spDeviceAllocateMemory(
    const SpiritDevice device,
    const size_t size,
    const u32 memoryType,
    VkDeviceMemory *memory)
{
    VkMemoryAllocateInfo allocationInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize  = size,
        .memoryTypeIndex = memoryType};

    if (vkAllocateMemory(
            device->device, &allocationInfo, ALLOCATION_CALLBACK, memory))
    {
        log_error("Failed to allocate memory");
        *memory = VK_NULL_HANDLE;
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}

SpiritResult spDeviceUpdateSwapchainSupport(const SpiritDevice device)
{
    if (device->swapchainDetails.presentModes)
        free(device->swapchainDetails.presentModes);
    if (device->swapchainDetails.formats)
        free(device->swapchainDetails.formats);

    device->swapchainDetails =
        querySwapChainSupport(device->windowSurface, device->physicalDevice);

    return SPIRIT_SUCCESS;
}

SpiritResult spDeviceCreateImage(
    const SpiritDevice device,
    const VkImageCreateInfo *imageInfo,
    VkMemoryPropertyFlags memoryFlags,
    VkImage *image,
    VkDeviceMemory *imageMemory)
{
    if (vkCreateImage(device->device, imageInfo, NULL, image))
        return SPIRIT_FAILURE;

    VkMemoryRequirements memoryRequirements = {};
    vkGetImageMemoryRequirements(device->device, *image, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize       = memoryRequirements.size;
    allocInfo.memoryTypeIndex      = spDeviceFindMemoryType(
        device, memoryRequirements.memoryTypeBits, memoryFlags);

    if (vkAllocateMemory(device->device, &allocInfo, NULL, imageMemory) !=
        VK_SUCCESS)
    {
        return SPIRIT_FAILURE;
    }

    if (vkBindImageMemory(device->device, *image, *imageMemory, 0) !=
        VK_SUCCESS)
    {
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}

SpiritResult spDeviceCreateBuffer(
    SpiritDevice device,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer *buffer,
    VkDeviceMemory *bufferMemory)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size               = size;
    bufferInfo.usage              = usage;
    bufferInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device->device, &bufferInfo, NULL, buffer) != VK_SUCCESS)
    {
        return SPIRIT_FAILURE;
    }

    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(device->device, *buffer, &req);

    u32 memType =
        spDeviceFindMemoryType(device, req.memoryTypeBits, properties);

    if (spDeviceAllocateMemory(device, req.size, memType, bufferMemory))
    {
        vkDestroyBuffer(device->device, *buffer, ALLOCATION_CALLBACK);
        return SPIRIT_FAILURE;
    }

    if (vkBindBufferMemory(device->device, *buffer, *bufferMemory, 0))
    {
        spDeviceFreeMemory(device, *bufferMemory);
        vkDestroyBuffer(device->device, *buffer, ALLOCATION_CALLBACK);
        return SPIRIT_FAILURE;
    }

    return SPIRIT_SUCCESS;
}

// destroy a spirit device and free all memory whatever
SpiritResult spDestroyDevice(SpiritDevice device)
{

    vkDestroyCommandPool(
        device->device, device->commandPool, ALLOCATION_CALLBACK);
    vkDestroyDevice(device->device, ALLOCATION_CALLBACK);

    vkDestroySurfaceKHR(device->instance, device->windowSurface, NULL);

    // debug messenger
    if (device->validationEnabled)
    {

        // load destroy function
        PFN_vkDestroyDebugUtilsMessengerEXT pfnDebugMessengerDestroy =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                device->instance, "vkDestroyDebugUtilsMessengerEXT");

        // if it fails
        if (pfnDebugMessengerDestroy == NULL)
            log_warning("Failed to load debug messenger destroy function");
        else
        {
            pfnDebugMessengerDestroy(
                device->instance, device->debugMessenger, ALLOCATION_CALLBACK);
        }
    }
    device->debugMessenger = NULL;

    vkDestroyInstance(device->instance, ALLOCATION_CALLBACK);

    free(device->swapchainDetails.formats);
    free(device->swapchainDetails.presentModes);

    free(device);

    return SPIRIT_SUCCESS;
}

//
// Private Helpers
//

// check if physical device is suitable for vulkan device
static u16 isDeviceSuitable(
    const SpiritDeviceCreateInfo *createInfo,
    VkPhysicalDevice questionedDevice);

// check available GPU extensions
static bool checkDeviceExtensionSupport(
    const SpiritDeviceCreateInfo *createInfo,
    VkPhysicalDevice questionedDevice);

//
// Helper Implementation
//

static VkInstance createInstance(
    const SpiritDeviceCreateInfo *createInfo,
    VkDebugUtilsMessengerEXT *debugMessenger)
{

    VkApplicationInfo appInfo  = {};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = createInfo->appName;
    appInfo.applicationVersion = createInfo->appVersion;
    appInfo.pEngineName        = createInfo->engineName;
    appInfo.engineVersion      = createInfo->engineVersion;
    appInfo.apiVersion         = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo     = &appInfo;

    // extensions
    u32 extensionCount = createInfo->windowExtensions.count;
    if (createInfo->enableValidation)
        extensionCount +=
            1; // if using validation, make room for valiation extensions
    const char *extensions[extensionCount];
    for (u32 i = 0;
         i < createInfo->windowExtensions.count && i < extensionCount;
         i++)
    {
        extensions[i] = createInfo->windowExtensions.names[i];
    }
    if (createInfo->enableValidation)
        extensions[extensionCount - 1] =
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME; // add debug extension if it
                                               // should
    instanceInfo.enabledExtensionCount   = extensionCount;
    instanceInfo.ppEnabledExtensionNames = extensions;

    // validation layers
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {}; // messenger create info
    const char *const *validationLayers          = NULL;
    instanceInfo.enabledLayerCount               = 0;
    if (createInfo->enableValidation)
    {
        log_verbose("Enabling Validation");

        // allocate memory for the debugmessenger if it is NULL
        db_assert_msg(debugMessenger, "debugMessenger was NULL, it should be a \
            reference to the &SpiritDevice.debugMessenger variable");
        // layer names and whatnot
        instanceInfo.enabledLayerCount =
            createInfo->requiredValidationLayerCount;
        validationLayers = createInfo->requiredValidationLayers;
        instanceInfo.enabledLayerCount =
            createInfo->requiredValidationLayerCount;
        instanceInfo.ppEnabledLayerNames = validationLayers;

        // debug messenger
        debugInfo.sType =
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        debugInfo.pfnUserCallback =
            (PFN_vkDebugUtilsMessengerCallbackEXT)debugCallback;

        instanceInfo.pNext = &debugInfo;
    }
    else
    {
        instanceInfo.enabledLayerCount   = 0;
        instanceInfo.ppEnabledLayerNames = NULL;
    }

    VkInstance instance;

    if (vkCreateInstance(&instanceInfo, NULL, &instance) != VK_SUCCESS)
    {
        return NULL;
    }

    if (createInfo->enableValidation)
    {
        PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugMesseger = NULL;
        pfnCreateDebugMesseger =
            (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                instance, "vkCreateDebugUtilsMessengerEXT");

        if (pfnCreateDebugMesseger == NULL)
            log_error(
                "Failed to load function "
                "vkCreateDebugUtilsMessengerEXT. Will log errors to stdout.");
        else
        {
            if (pfnCreateDebugMesseger(
                    instance, &debugInfo, NULL, debugMessenger) != VK_SUCCESS)
            {
                log_error("Failed to create debug messenger");
                *debugMessenger = NULL;
            }
        }
    }

    log_verbose("Created Vulkan Instance");
    return instance;
}

static VkPhysicalDevice selectPhysicalDevice(
    const SpiritDeviceCreateInfo *createInfo, const VkInstance instance)
{

    // check available rendering devices
    u32 availableDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &availableDeviceCount, NULL);
    // throw error if there are no devices
    if (availableDeviceCount == 0)
    {
        log_fatal("No GPU detected.");
        return NULL;
    }

    VkPhysicalDevice availableDevices[availableDeviceCount];
    vkEnumeratePhysicalDevices(
        instance, &availableDeviceCount, availableDevices);

    u32 suitableDeviceCount = 0;
    SuitableDevice suitableDevices[availableDeviceCount];

    uint16_t tmpDeviceOutput = 0; // used to store function output in for loop
    for (u32 i = 0; i < availableDeviceCount; i++)
    {
        tmpDeviceOutput = isDeviceSuitable(createInfo, availableDevices[i]);
        if (tmpDeviceOutput > 0)
        {
            suitableDevices[suitableDeviceCount].physicalDevice =
                availableDevices[i];
            suitableDevices[suitableDeviceCount].deviceScore = tmpDeviceOutput;
            suitableDeviceCount++;
        }
    }

    // check if there are no suitable GPU
    if (suitableDeviceCount == 0)
    {
        log_fatal("No suitable GPU detected.");
        return NULL;
    }

    // select highest ranking device
    SuitableDevice outputDevice;
    outputDevice.deviceScore    = 0;
    outputDevice.physicalDevice = VK_NULL_HANDLE;
    for (uint16_t i = 0; i < suitableDeviceCount; i++)
    {

        if (suitableDevices[i].deviceScore > outputDevice.deviceScore)
        {
            outputDevice.deviceScore    = suitableDevices[i].deviceScore;
            outputDevice.physicalDevice = suitableDevices[i].physicalDevice;
        }
    }

    // log selected device name
#ifdef ENABLE_VERBOSE
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(outputDevice.physicalDevice, &properties);
    log_info("Selected GPU '%s'", properties.deviceName);
#endif
    // set device to highest ranking device
    return outputDevice.physicalDevice;
}

static VkDevice createDevice(
    const SpiritDeviceCreateInfo *createInfo,
    const VkPhysicalDevice physicalDevice)
{

    QueueFamilyIndices indices = findDeviceQueues(createInfo, physicalDevice);

    // create queues
    f32 queuePriority = 1.0f; // float to be referenced (&queuePriority)
    u32 queueCount    = QUEUE_COUNT;
    u32 queueFamilies[QUEUE_COUNT] = QUEUE_NAMES(indices);
    VkDeviceQueueCreateInfo queueCreateInfos[queueCount];
    u32 addedQueues[QUEUE_COUNT];
    u32 skippedQueueCount = 0;
    for (u32 i = 0; i < queueCount; i++)
    {
        bool isDuplicateQueue = false;
        // avoid adding duplicate queues
        // the < i is to ensure that we don't check indexes of
        // addedQueues that have not been set yet (false positives with 0)
        for (u32 n = 0; n < queueCount && n < i; n++)
        {
            if (queueFamilies[i] == addedQueues[n])
            {
                isDuplicateQueue = true;
            }
        }
        if (isDuplicateQueue)
        {
            skippedQueueCount++;
            continue;
        }
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilies[i];
        queueCreateInfo.queueCount       = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        // subtract skipped queues to avoid blank spaces
        // [set, set, duplicate, set] -> [set, set, set]
        queueCreateInfos[i - skippedQueueCount] = queueCreateInfo;
        addedQueues[i - skippedQueueCount]      = queueFamilies[i];
    }

    VkPhysicalDeviceFeatures deviceFeatures = {}; // populate later

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType              = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = queueCount - skippedQueueCount;
    deviceCreateInfo.pQueueCreateInfos    = queueCreateInfos;

    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    // enable device extensions
    deviceCreateInfo.enabledExtensionCount =
        createInfo->requiredDeviceExtensionCount;
    deviceCreateInfo.ppEnabledExtensionNames =
        createInfo->requiredDeviceExtensions;

    // pass used enabled validation layers
    deviceCreateInfo.enabledLayerCount =
        createInfo->requiredValidationLayerCount;
    deviceCreateInfo.ppEnabledLayerNames = createInfo->requiredValidationLayers;

    VkDevice device;
    VkResult errCode;
    if ((errCode =
             vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device)))
    {
        // debug function
        log_info("Device failure code '%d'", errCode);
        return NULL;
    }

    log_verbose("Created logical device");
    return device;
}

static VkCommandPool
createCommandPool(VkDevice device, QueueFamilyIndices queueFamilies)
{

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilies.graphicsQueue;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
                     VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool commandPool = VK_NULL_HANDLE;
    if (vkCreateCommandPool(device, &poolInfo, NULL, &commandPool) !=
        VK_SUCCESS)
    {
        log_error("Failed to create command pool");
    }

    return commandPool;
}

// debug callback function
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes __attribute_maybe_unused__,
    VkDebugUtilsMessengerCallbackDataEXT *pCallbackData
        __attribute_maybe_unused__,
    void *pUserData __attribute_maybe_unused__)
{

    if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        log_validation("%s", pCallbackData->pMessage);
        return true;
    }
    else
        return false;
}

//
// Private Helpers
//

// instance
static bool checkValidationLayerSupport(
    const char *const *requiredLayerNames, u32 requiredLayerCount)
{

    u32 layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties availableLayers[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (u32 i = 0; i < requiredLayerCount; i++)
    {
        bool found = false;

        // iterate through available extensions
        for (u32 x = 0; x < layerCount; x++)
        {
            if (strncmp(
                    requiredLayerNames[i], availableLayers[x].layerName, 256) ==
                0)
            {
                found = true;
                break;
            }
        }

        if (found == false)
        {
            log_warning(
                "Unavailable validation layer: %s", requiredLayerNames[i]);
            return false;
        }
    }

    return true;
}

// Physical device

// function to check if a physical device meets requirements
static u16 isDeviceSuitable(
    const SpiritDeviceCreateInfo *createInfo, VkPhysicalDevice questionedDevice)
{

    // get random device info
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(questionedDevice, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(questionedDevice, &deviceFeatures);

    uint16_t score = 0; // the device priority when selecting in future

    // requirment bools
    bool hasExtensionSupport = false; // if device support required extensions
    bool supportsQueues      = false; // if device supports required queues
    bool supportsSwapchain =
        false; // if device supports adequate swapchain functionality

    // check if device supports required extensions
    if (checkDeviceExtensionSupport(createInfo, questionedDevice) == true)
    {
        hasExtensionSupport = true;
    }

    // check if device support graphics queue and presentation support
    QueueFamilyIndices deviceQueue = {};
    deviceQueue = findDeviceQueues(createInfo, questionedDevice);
    if (deviceQueue.foundGraphicsQueue && deviceQueue.foundPresentQueue)
    {
        supportsQueues = true;
    }

    // check if device has swapchain support
    SpiritSwapchainSupportInfo swapchainDetails =
        querySwapChainSupport(createInfo->windowSurface, questionedDevice);
    supportsSwapchain = swapchainDetails.formatCount > 0 &&
                        swapchainDetails.presentModeCount > 0;

    free(swapchainDetails.formats);
    free(swapchainDetails.presentModes);

    // DEBUG
    if (!supportsQueues)
        log_error(
            "Device '%s' lacks queue support", deviceProperties.deviceName);
    if (!hasExtensionSupport)
        log_error(
            "Device '%s' lacks extension support", deviceProperties.deviceName);
    if (!supportsSwapchain)
        log_error(
            "Device '%s' lacks swapchain support", deviceProperties.deviceName);

    // check if device supports all functionality
    if (hasExtensionSupport && supportsQueues && supportsSwapchain)
    {
        score = 1;
    }

    // score device based on type of gpu
    if (score == 0)
    {
        log_verbose(
            "Device '%s' has score %i", deviceProperties.deviceName, score);
        return 0;
    }
    else if (
        deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 4;
    }
    else if (
        deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
    {
        // increase priority for integrated GPU if wanted
        if (createInfo->powerSaveMode)
        {
            score += 3;
        }
        score += 2;
    }
    else
    {
        score = 0;
    }

    // increase gpu score if same queue supports everything
    if (deviceQueue.graphicsQueue == deviceQueue.presentQueue)
    {
        score += 1;
    }
    return score;
}

// function to check if a physical device supports swapchain
static bool checkDeviceExtensionSupport(
    const SpiritDeviceCreateInfo *createInfo, VkPhysicalDevice questionedDevice)
{

    u32 availailableExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(
        questionedDevice, NULL, &availailableExtensionCount, NULL);
    VkExtensionProperties availableExtensionNames[availailableExtensionCount];
    vkEnumerateDeviceExtensionProperties(
        questionedDevice,
        NULL,
        &availailableExtensionCount,
        availableExtensionNames);
    // highly efficient iterator (two means twice the speed)
    for (u32 i = 0; i < createInfo->requiredDeviceExtensionCount; i++)
    {
        bool found = false; // if the current extension has been found
        for (u32 x = 0; x < availailableExtensionCount; x++)
        {
            if (strcmp(
                    createInfo->requiredDeviceExtensions[i],
                    availableExtensionNames[x].extensionName) == 0)
            {
                found = true;
                break;
            }
        }
        // "handle" missing layer
        if (!found)
        {
            log_fatal(
                "GPU does not support required extension: %s",
                createInfo->requiredDeviceExtensions[i]);
            return false;
        }
    }

    return true;
}

// helper function to access required device queues
static QueueFamilyIndices findDeviceQueues(
    const SpiritDeviceCreateInfo *createInfo, VkPhysicalDevice questionedDevice)
{

    QueueFamilyIndices indices = {};

    // get device queue properties
    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        questionedDevice, &queueFamilyCount, NULL);
    VkQueueFamilyProperties deviceQueueProperties[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(
        questionedDevice, &queueFamilyCount, deviceQueueProperties);

    // check for desired queue
    VkBool32 presentationSupport = false;
    for (u32 i = 0; i < queueFamilyCount; i++)
    {

        // check for graphics queue support if not already found
        if (!indices.foundGraphicsQueue)
        {
            if (deviceQueueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsQueue      = i;
                indices.foundGraphicsQueue = true;
            }
        }

        // check for presentationSupport if not already found
        if (!indices.foundPresentQueue)
        {
            if (vkGetPhysicalDeviceSurfaceSupportKHR(
                    questionedDevice,
                    i,
                    createInfo->windowSurface,
                    &presentationSupport) != VK_SUCCESS)
                log_info("Failed find presentation support!");

            if (presentationSupport)
            {
                assert(presentationSupport);
                indices.foundPresentQueue = presentationSupport;
                indices.presentQueue      = i;
            }
        }

        // check if all queues found
        if (indices.foundPresentQueue && indices.foundGraphicsQueue)
        {
            break;
        }
    }

    return indices;
}

// function to check swapchain support
SpiritSwapchainSupportInfo querySwapChainSupport(
    const VkSurfaceKHR surface, VkPhysicalDevice questionedDevice)
{
    SpiritSwapchainSupportInfo details = {};

    // check surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        questionedDevice, surface, &details.capabilties);

    // formats
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        questionedDevice, surface, &details.formatCount, NULL);
    db_assert_msg(details.formatCount, "Device surface has no format.");
    details.formats = new_array(VkSurfaceFormatKHR, details.formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        questionedDevice, surface, &details.formatCount, details.formats);
    db_assert_msg(details.formats, "Device surface has no formats");

    if (details.formatCount == 0)
    {
        details.formats = NULL;
    }

    // present modes
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        questionedDevice, surface, &details.presentModeCount, NULL);
    db_assert_msg(
        details.presentModeCount > 0, "Device surface has no present modes");
    details.presentModes =
        new_array(VkPresentModeKHR, details.presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        questionedDevice,
        surface,
        &details.presentModeCount,
        details.presentModes);
    db_assert_msg(
        details.presentModes != NULL, "Device surface has no present modes");

    return details;
}
