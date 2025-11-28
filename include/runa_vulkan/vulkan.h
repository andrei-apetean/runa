#ifndef RVULKAN_H
#define RVULKAN_H

#include <vulkan/vulkan.h>


struct rvulkan {
    VkInstance instance;
    VkAllocationCallbacks* callbacks;
#ifdef _DEBUG
    VkDebugUtilsMessengerEXT messenger;
#endif /* _DEBUG */
};

int init_vulkan(struct rvulkan* self);
void terminate_vulkan(struct rvulkan* self);

#endif
