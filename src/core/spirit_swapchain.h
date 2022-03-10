#pragma once
#include <spirit_header.h>

// swapchain creation and recreation
//
//
// Kael Johnston Mar 10 2022

SpiritSwapchain spCreateSwapchain (SpiritSwapchainCreateInfo createInfo, SpiritDevice device);

SpiritResult spSwapchainRecreate (SpiritSwapchain swapchain, SpiritDevice device);

SpiritResult spDestroySwapchain (SpiritSwapchain, SpiritDevice);
