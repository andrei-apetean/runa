#include <string.h>

#include "runa/runa.h"

#include "module.h"

struct vulkan_module {
    struct runa_itf itf;
    struct vulkan vk;
};

int vulkan_module_on_init(struct runa_itf* runa_itf)
{
    struct vulkan_module* self = container_of(runa_itf, struct vulkan_module, itf);
    struct vulkan* vk = &self->vk;
    self->vk.log("%s (%p)\n", __FUNCTION__, vk);
    int err = 0;
    err = init_instance(vk);
    if (err) {
        self->vk.log("Failed to initialize vulkan instance!\n");
        return err;
    }
#if _DEBUG
    err = create_debug_messenger(vk);
#endif /* _DEBUG */
    return err;
return 0;
}

void vulkan_module_on_terminate(struct runa_itf* runa_itf)
{
    struct vulkan_module* self = container_of(runa_itf, struct vulkan_module, itf);
    struct vulkan* vk = &self->vk;
    vk->log("%s (%p)\n", __FUNCTION__, vk);
}

void vulkan_module_on_update(struct runa_itf* runa_itf)
{
    struct vulkan_module* self = container_of(runa_itf, struct vulkan_module, itf);
    struct vulkan* vk = &self->vk;
    self->vk.log("%s (%p)\n", __FUNCTION__, vk);

}

struct runa_itf* vulkan_module_on_load(void* mem, const struct runa* const runa)
{
    if (!mem || !runa) {
        return 0;
    }
    struct vulkan_module* self = mem;
    self->itf.on_init = vulkan_module_on_init;
    self->itf.on_terminate = vulkan_module_on_terminate;
    self->itf.on_update = vulkan_module_on_update;
    self->vk.log = runa->log;
    struct vulkan* vk = &self->vk;
    self->vk.log("%s (%p)\n", __FUNCTION__, vk);
    return &self->itf;
}

void get_modinfo(struct runa_itfd* descriptor)
{
    descriptor->size_bytes = sizeof(struct vulkan_module);
    strcpy(descriptor->name, "Runa Vulkan"); /* TODO: prolly not safe */
    descriptor->load = vulkan_module_on_load;
}

