#ifndef VULKAN_MODULE_H
#define VULKAN_MODULE_H

#include <vulkan/vulkan.h>

struct vulkan {
    VkInstance instance;
    VkAllocationCallbacks* callbacks;
#ifdef _DEBUG
    VkDebugUtilsMessengerEXT messenger;
#endif /* _DEBUG */
    int (*log)(const char*, ...);
    char arr[122];
};

int init_instance(struct vulkan* self);

#if _DEBUG
int create_debug_messenger(struct vulkan* self);
void destroy_debug_messenger(struct vulkan* self);
#endif /* _DEBUG */

#endif /* VULKAN_MODULE_H */
