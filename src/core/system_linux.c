#ifdef __linux__
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <wayland-client.h>
#include "wl/xdg-shell.h"

#include "core.h"
#include "log.h"
#include "time.h"
#include "system.h"

#define ONE_MILLION 1000000

struct linux_connection {
    int id;
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct xdg_wm_base* shell;
    struct xdg_shell* xdg_shell;
    struct wl_surface* surface;
    struct xdg_surface* xdg_surface;
    struct xdg_toplevel* toplevel;

    int width;
    int height;
};

uint64_t timestamp(void)
{
    struct timeval val;
    gettimeofday(&val, NULL);
    return (uint64_t)val.tv_usec + ((uint64_t)val.tv_sec * ONE_MILLION);
}

void ssleep(uint64_t us) 
{
    usleep(us);
}

static void shell_ping(void* data, struct xdg_wm_base* xdg_wm_base, uint32_t serial) {
    (void)data;
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener shell_listener = {
    .ping = shell_ping,
};

static void setup_registry(void* data, struct wl_registry* registry, 
                           uint32_t name, const char* interface, 
                           uint32_t version)
{
    (void)registry;
    (void)data;
    (void)name;
    (void)interface;
    (void)version;
    struct linux_connection* self = data;
     if (strcmp(interface, wl_compositor_interface.name) == 0) {
        self->compositor = wl_registry_bind(self->registry, name, 
                                            &wl_compositor_interface, 1);
    }
    if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        self->shell = wl_registry_bind(self->registry, name, 
                                       &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(self->shell, &shell_listener, self);
        return;
    }

}

static void xdg_shell_configure(void* data, struct xdg_surface* xdg_surface, uint32_t serial) {
    xdg_surface_ack_configure(xdg_surface, serial);
    (void)data;
}

static void toplevel_configure(void* data, struct xdg_toplevel* toplevel,
                            int32_t width, int32_t height,
                            struct wl_array* states) {
    /* TODO: handle resize */;
    (void)data;
    (void)toplevel;
    (void)width;
    (void)height;
    (void)states;
}
static void toplevel_close(void* data, struct xdg_toplevel* toplevel) {
    (void)toplevel;
    (void)data;
    // self->request_shutdown->invoke(self->request_shutdown);
}

static const struct wl_registry_listener registry_listener = {
    .global = setup_registry,
};

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_shell_configure,
};

static const struct xdg_toplevel_listener toplevel_listener = {
    .configure = toplevel_configure,
    .close = toplevel_close,
};

int system_connect(struct syscon* connection)
{
    // TODO: custom allocator
    struct linux_connection* self = malloc(sizeof(*self));
    // TODO: remove string.h
    if (!self) return -1;

    memset(self, 0, sizeof(*self));
    connection->id = &self->id;

    self->display = wl_display_connect(0);
    if (!self->display) goto _err;

    self->registry = wl_display_get_registry(self->display);
    if (!self->registry) goto _err;
    wl_registry_add_listener(self->registry, &registry_listener, self);

    wl_display_roundtrip(self->display);
    if (!self->compositor || !self->shell) goto _err;

     self->surface = wl_compositor_create_surface(self->compositor);
    if (!self->surface) goto _err;

    self->xdg_surface = xdg_wm_base_get_xdg_surface(self->shell, self->surface);
    if (!self->xdg_surface) goto _err;

    xdg_surface_add_listener(self->xdg_surface, &xdg_surface_listener, self);

    self->toplevel = xdg_surface_get_toplevel(self->xdg_surface);
    if (!self->toplevel) goto _err;

    xdg_toplevel_add_listener(self->toplevel, &toplevel_listener, self);
    xdg_toplevel_set_title(self->toplevel, "runa");
    xdg_toplevel_set_app_id(self->toplevel, "runa");

    self->width = 1920;
    self->height = 1080;

    wl_surface_commit(self->surface);
    /* end window stuff */
    wl_display_roundtrip(self->display);
    wl_display_roundtrip(self->display);
    return 0;

 _err:
    if (self->toplevel) xdg_toplevel_destroy(self->toplevel);
    if (self->xdg_surface) xdg_surface_destroy(self->xdg_surface);
    if (self->surface) wl_surface_destroy(self->surface);
    if (self->shell) xdg_wm_base_destroy(self->shell);
    if (self->compositor) wl_compositor_destroy(self->compositor);
    if (self->registry) wl_registry_destroy(self->registry);
    if (self->display) wl_display_disconnect(self->display);
    free(self);
    return -1;
}

void system_read_events(struct syscon* connection)
{
    struct linux_connection* self = container_of(connection->id, struct linux_connection, id);
    wl_display_dispatch_pending(self->display);
    while (wl_display_prepare_read(self->display)) {
        wl_display_dispatch_pending(self->display);
    }

    if (wl_display_read_events(self->display)) {
        logwrn("Failed to read wayland events");
    }
    wl_display_dispatch_pending(self->display);
    wl_display_flush(self->display);
}

void system_disconnect(struct syscon* connection)
{
    (void)connection;
}

enum surface_api system_surface_api()
{
    return SURFACE_API_WL;
}

#endif // __linux__
