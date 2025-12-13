#include <dirent.h>
#include <libgen.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <dlfcn.h>

#include <wayland-client.h>
#include <wl/xdg-shell.h>

#include "runa_core/core.h"

#define ONE_MILLION 1000000
#define PATH_SZ 1024

static char modules_dir[PATH_SZ] = {0};
static int module_count;
static int running = 0;

/* TODO: not sure what to do about these */
static int width;
static int height;

static struct rt rt;
static struct wl_display* display;
static struct wl_registry* registry;
static struct wl_compositor* compositor;
static struct xdg_wm_base* shell;
// static struct xdg_shell* xdg_shell;
static struct wl_surface* surface;
static struct xdg_surface* xdg_surface;
static struct xdg_toplevel* toplevel;

static uint64_t timestamp(void)
{
    struct timeval val;
    gettimeofday(&val, NULL);
    return (uint64_t)val.tv_usec + ((uint64_t)val.tv_sec * ONE_MILLION);
}

static void idle(uint64_t us) 
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
        compositor = wl_registry_bind(registry, name, 
                                            &wl_compositor_interface, 1);
    }
    if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        shell = wl_registry_bind(registry, name, 
                                       &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(shell, &shell_listener, self);
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
    running = 0;
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

static void disconnect()
{
    if (toplevel) xdg_toplevel_destroy(toplevel);
    if (xdg_surface) xdg_surface_destroy(xdg_surface);
    if (surface) wl_surface_destroy(surface);
    if (shell) xdg_wm_base_destroy(shell);
    if (compositor) wl_compositor_destroy(compositor);
    if (registry) wl_registry_destroy(registry);
    if (display) wl_display_disconnect(display);
}

static int connect()
{
    display = wl_display_connect(0);
    if (!display) goto _err;

    registry = wl_display_get_registry(display);
    if (!registry) goto _err;
    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_roundtrip(display);
    if (!compositor || !shell) goto _err;

     surface = wl_compositor_create_surface(compositor);
    if (!surface) goto _err;

    xdg_surface = xdg_wm_base_get_xdg_surface(shell, surface);
    if (!xdg_surface) goto _err;

    xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, NULL);

    toplevel = xdg_surface_get_toplevel(xdg_surface);
    if (!toplevel) goto _err;

    xdg_toplevel_add_listener(toplevel, &toplevel_listener, NULL);
    xdg_toplevel_set_title(toplevel, "runa");
    xdg_toplevel_set_app_id(toplevel, "runa");

    width = 1920;
    height = 1080;

    wl_surface_commit(surface);
    /* end window stuff */
    wl_display_roundtrip(display);
    wl_display_roundtrip(display);
    return 0;

 _err:
    disconnect();
    return -1;
}

static int read_events(/* TODO pass the events*/)
{
    wl_display_dispatch_pending(display);
    while (wl_display_prepare_read(display)) {
        wl_display_dispatch_pending(display);
    }

    if (wl_display_read_events(display)) {
        printf("Failed to read wayland events");
    }
    wl_display_dispatch_pending(display);
    wl_display_flush(display);
    return 0;
}

static int load_module_info(int* count, struct module** mods)
{
    DIR* d = opendir(modules_dir);
    if (!d) return -1;

    *count = -2;
    struct dirent* entry;
    while ((entry = readdir(d))) (*count)++;

    rewinddir(d);
    struct module* modules = malloc(sizeof(*modules) * (*count));
    if (!modules) return -1;

    printf("Reading modules...\n");
    int i = 0;
    while ((entry = readdir(d))) {
        if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name)) continue;

        char mod_path[PATH_SZ] = { 0 };
        sprintf(mod_path, "%s/%s", modules_dir, entry->d_name);

        void* lib = dlopen(mod_path, RTLD_NOW);
        if (!lib) continue;

        void(*get_info)(struct runa_itfd*) = dlsym(lib, "get_modinfo");
        if (!get_info) {
            dlclose(lib);
            continue;
        }
        struct module* m = &modules[i];
        *m = (struct module){0};
        get_info(&m->descriptor);
        m->handle = (uintptr_t)lib;
        printf("%s [%s] (%d bytes)\n",
               m->descriptor.name, entry->d_name, m->descriptor.size_bytes);
        i++;
    }

    closedir(d);
    (*mods) = modules;
    return 0;
}

int main()
{
    int err = 0; 

    {
        char self_path[1024] = {0};
        err = readlink("/proc/self/exe", self_path, sizeof(self_path)-1);
        if (err < 0) {
            return err;
        }
        err = 0;
        char* self_dir = dirname(self_path);
        sprintf(modules_dir, "%s/modules", self_dir);
    }

    err = connect();
    if (err) {
        printf("Failed to connect to the system!\n");
        return err;
    }

    rt = (struct rt) {
        .timestamp = timestamp,
        .idle = idle,
        .log = printf,
        .read_events = read_events,
        .modcount = module_count,
    };
    err = load_module_info(&rt.modcount, &rt.modules);

    err = init(&rt);
    if (err) {
        printf("Failed to connect to the system!\n");
        return err;
    }


    printf("Initialzation successful!\n");
    volatile int i = 0;
    running = 1;
    while (running) {
        loop(&rt);
        i++;
        if (i > 0){
            running = 0;
        }
    }
    terminate(&rt);
    free(rt.modules);
    disconnect();
}
