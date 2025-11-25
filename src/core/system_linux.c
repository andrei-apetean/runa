#include "log.h"
#ifdef __linux__

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <wayland-client.h>
#include "wl/xdg-shell.h"

#include "time.h"
#include "core.h"

#define ONE_MILLION 1000000

struct linux_connection {
    int id;
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct xdg_wm_base* shell;
    struct xdg_shell* xdg_shell;
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

static const struct wl_registry_listener registry_listener = {
    .global = setup_registry,
};

int system_connect(struct syscon* connection)
{
    // TODO: custom allocator
    struct linux_connection* con = malloc(sizeof(*con));
    // TODO: remove string.h
    if (!con) return -1;

    memset(con, 0, sizeof(*con));
    connection->id = &con->id;

    con->display = wl_display_connect(0);
    if (!con->display) goto _err;

    con->registry = wl_display_get_registry(con->display);
    if (!con->registry) goto _err;
    wl_registry_add_listener(con->registry, &registry_listener, con);

    wl_display_roundtrip(con->display);
    if (!con->compositor || !con->shell) goto _err;

    return 0;

 _err:
    if (con->shell) xdg_wm_base_destroy(con->shell);
    if (con->compositor) wl_compositor_destroy(con->compositor);
    if (con->registry) wl_registry_destroy(con->registry);
    if (con->display) wl_display_disconnect(con->display);
    free(con);
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

#endif // __linux__
