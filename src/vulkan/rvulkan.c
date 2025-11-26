#include "rvulkan.h"
#include "log.h"
#include "system.h"

static int init_instance(struct rvulkan* self);

#if _DEBUG
static int create_debug_messenger(struct rvulkan* self);
static void destroy_debug_messenger(struct rvulkan* self);
#endif /* _DEBUG */

int init_vulkan(struct syscon* connection, struct rvulkan* self) 
{
    (void)connection;
    int err = 0;
    err = init_instance(self);
    if (err) {
        logerr("Failed to initialize vulkan instance!\n");
        return err;
    }
#if _DEBUG
    err = create_debug_messenger(self);
    if (err) logwrn("Failed to create vulkan debug messenger!\n");
#endif /* _DEBUG */
    return err;
}

void terminate_vulkan(struct rvulkan* self)
{
#if _DEBUG
    destroy_debug_messenger(self);
    vkDestroyInstance(self->instance, self->callbacks);
#endif /* _DEBUG */
}

static int init_instance(struct rvulkan* self)
{
    (void)self;
    const char* surface_extensions[SURFACE_API_COUNT] = {
        [SURFACE_API_WL] = "VK_KHR_wayland_surface",
    };
    enum surface_api surface_api = system_surface_api();

    const char* extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        /* probably should check the return value is less than max */
        surface_extensions[surface_api],
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
        loginf("\t%s\n", extensions[i]);
    }
    for (int i = 0; i < layer_count; i++) {
        loginf("\t%s\n", layers[i]);
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
    (void)user_data;
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        logerr(" MessageID: %s %i\nMessage: %s\n\n", data->pMessageIdName,
              data->messageIdNumber, data->pMessage);
    }

    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        logwrn(" MessageID: %s %i\nMessage: %s\n\n", data->pMessageIdName,
              data->messageIdNumber, data->pMessage);
    }

    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        loginf(" MessageID: %s %i\nMessage: %s\n\n", data->pMessageIdName,
              data->messageIdNumber, data->pMessage);
    }

    return VK_FALSE;
}
static int create_debug_messenger(struct rvulkan* self)
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

static void destroy_debug_messenger(struct rvulkan* self)
{
    if (!self->messenger) return;
    PFN_vkDestroyDebugUtilsMessengerEXT destroy = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr( self->instance, "vkDestroyDebugUtilsMessengerEXT");

    if (!destroy) {
        logerr("Failed to load debug messanger destruction function!");
        return;
    }
    destroy(self->instance, self->messenger, self->callbacks);
}
#endif /* _DEBUG */
