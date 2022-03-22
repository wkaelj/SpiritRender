#pragma once
#include <spirit_header.h>

// swapchain creation and recreation
//
//
// Kael Johnston Mar 10 2022

// create a new swapchain. Can optionaly pass a old swapchain, which will be recreated with new window resolution
SpiritSwapchain spCreateSwapchain (SpiritSwapchainCreateInfo createInfo, SpiritDevice device, SpiritSwapchain optionalSwapchain);

// destroy a swapchain and free associated memory
SpiritResult spDestroySwapchain (SpiritSwapchain swapchain, SpiritDevice device);
