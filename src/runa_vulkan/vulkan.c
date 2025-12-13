#include "runa/runa.h"

#include "module.h"

void terminate_vulkan(struct vulkan* self)
{
#if _DEBUG
    destroy_debug_messenger(self);
    vkDestroyInstance(self->instance, self->callbacks);
#endif /* _DEBUG */
}

int init_instance(struct vulkan* self)
{
    /* TODO */
    const char* surface_extensions[1] = {
        [0] = "VK_KHR_wayland_surface",
    };

    const char* extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        /* probably should check the return value is less than max */
        surface_extensions[0], /* TODO */
#if _DEBUG
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif /* _DEBUG */
    };
    
    const char* layers[] = {
#if _DEBUG
    "VK_LAYER_KHRONOS_validation",
#endif /* _DEBUG */
    };

    int extension_count = count_of(extensions);
    int layer_count = count_of(layers);

#if _DEBUG
    
    for (int i = 0; i < extension_count; i++) {
        self->log("\t%s\n", extensions[i]);
    }
    for (int i = 0; i < layer_count; i++) {
        self->log("\t%s\n", layers[i]);
    }
   
#endif /* _DEBUG */

    /* TODO: verify extensions/layers available */
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "runa",
        .pEngineName = "runa",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_MAKE_VERSION(1, 3, 0),
    };

    VkInstanceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = layer_count,
        .enabledExtensionCount = extension_count,
        .ppEnabledLayerNames = layers,
        .ppEnabledExtensionNames = extensions,
    };
    VkResult result = vkCreateInstance(&info, self->callbacks, &self->instance);
    return result == VK_SUCCESS ? 0 : -1;
}

#if _DEBUG

static VkBool32 debug_utils_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
    VkDebugUtilsMessageTypeFlagsEXT             types,
    const VkDebugUtilsMessengerCallbackDataEXT* data, void* user_data) {
    (void)types;
    (void)severity;
    (void)data;
    (void)user_data;
    /* if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    *     logerr(" MessageID: %s %i\nMessage: %s\n\n", data->pMessageIdName,
    *           data->messageIdNumber, data->pMessage);
    * }

    * if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    *     logwrn(" MessageID: %s %i\nMessage: %s\n\n", data->pMessageIdName,
    *           data->messageIdNumber, data->pMessage);
    * }

    * if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    *     loginf(" MessageID: %s %i\nMessage: %s\n\n", data->pMessageIdName,
    *           data->messageIdNumber, data->pMessage);
    * }
    */

    return VK_FALSE;
}

int create_debug_messenger(struct vulkan* self)
{
    const VkDebugUtilsMessengerCreateInfoEXT info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT ,
        .pfnUserCallback = debug_utils_callback,
    };
    PFN_vkCreateDebugUtilsMessengerEXT create = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(self->instance, "vkCreateDebugUtilsMessengerEXT");

    if (!create) return VK_ERROR_UNKNOWN;

    return create(self->instance, &info, 0, &self->messenger);
}

void destroy_debug_messenger(struct vulkan* self)
{
    if (!self->messenger) return;
    PFN_vkDestroyDebugUtilsMessengerEXT destroy = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr( self->instance, "vkDestroyDebugUtilsMessengerEXT");

    if (!destroy) {
        // logerr("Failed to load debug messanger destruction function!");
        return;
    }
    destroy(self->instance, self->messenger, self->callbacks);
}
#endif /* _DEBUG */
