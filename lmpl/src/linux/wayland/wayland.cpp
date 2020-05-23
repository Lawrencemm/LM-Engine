#include <cstdlib>
#include <string>

#include <wayland-client.h>

#include <lmlib/application.h>

#include "wayland.h"

using namespace std::literals::string_literals;

namespace lmpl
{

static void
  seat_handle_capabilities(void *data, struct wl_seat *seat, uint32_t caps)
{
    if (caps & WL_SEAT_CAPABILITY_POINTER)
    {
        printf("Display has a pointer\n");
    }

    if (caps & WL_SEAT_CAPABILITY_KEYBOARD)
    {
        printf("Display has a keyboard\n");
    }

    if (caps & WL_SEAT_CAPABILITY_TOUCH)
    {
        printf("Display has a touch screen\n");
    }
}

static const struct wl_seat_listener seat_listener = {
  seat_handle_capabilities,
};

static void global_registry_handler(
  void *data,
  struct wl_registry *registry,
  uint32_t id,
  const char *interface,
  uint32_t version)
{
    auto display = (WaylandDisplay *)data;
    DEBUG_OUTPUT("Got a Wayland registry event for "s + interface + ".\n");
    if (interface == "wl_compositor"s)
    {
        display->compositor = (wl_compositor *)wl_registry_bind(
          display->registry, id, &wl_compositor_interface, 1);
    }
    else if (interface == "wl_shell"s)
    {
        display->shell =
          (wl_shell *)wl_registry_bind(registry, id, &wl_shell_interface, 1);
    }
    else if (interface == "wl_seat"s)
    {
        display->seat =
          (wl_seat *)wl_registry_bind(registry, id, &wl_seat_interface, 1);
        wl_seat_add_listener(display->seat, &seat_listener, NULL);
    }
}

static void
  global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    return;
}

static void handle_ping(
  void *data,
  struct wl_shell_surface *shell_surface,
  uint32_t serial)
{
    wl_shell_surface_pong(shell_surface, serial);
    DEBUG_OUTPUT_LINE("Pinged.");
}

void WaylandWindow::handle_configure(
  void *data,
  struct wl_shell_surface *shell_surface,
  uint32_t edges,
  int32_t width,
  int32_t height)
{
    auto p_window = (WaylandWindow *)data;
    // p_window->resize_handler(*p_window);
}

static void
  handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
}

static const struct wl_shell_surface_listener shell_surface_listener = {
  handle_ping,
  WaylandWindow::handle_configure,
  handle_popup_done};

WaylandDisplay::WaylandDisplay()
    : display{wl_display_connect(nullptr)},
      registry{nullptr},
      compositor{nullptr}
{
}

WaylandDisplay &WaylandDisplay::init()
{
    registry = wl_display_get_registry(display);
    auto display_env_var = getenv("WAYLAND_DISPLAY");

    if (!display)
    {
        if (!display_env_var)
            throw NoWaylandEnvVar{};
        else
            throw WaylandDisplayError{};
    }

    static const struct wl_registry_listener registry_listener = {
      global_registry_handler, global_registry_remover};
    wl_registry_add_listener(registry, &registry_listener, this);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    if (!compositor)
    {
        throw NoCompositor{};
    }

    if (!shell)
    {
        throw NoShell{};
    }

    return *this;
}

WaylandWindow &WaylandWindow::init(WaylandDisplay &display)
{
    p_display_ = &display;
    surface = wl_compositor_create_surface(display.compositor);
    if (!surface)
    {
        throw WindowSurfaceNull{};
    }
    shell_surface = wl_shell_get_shell_surface(display.shell, surface);
    if (!shell_surface)
    {
        throw WindowShellSurfaceNull{};
    }

    wl_shell_surface_add_listener(shell_surface, &shell_surface_listener, this);
    return *this;
}

WaylandWindow &WaylandWindow::show()
{
    wl_shell_surface_set_toplevel(shell_surface);
    return *this;
}

WaylandWindow &WaylandWindow::size(Size new_size)
{
    this->desired_size = new_size;
    return *this;
}

// vk::UniqueHandle<vk::SurfaceKHR>
// WaylandWindow::vulkan_surface(vulkan::Renderer& renderer)
//{
//  auto create_info = vk::WaylandSurfaceCreateInfoKHR{}
//    .setDisplay(p_display_->display)
//    .setSurface(surface)
//  ;
//
//  return renderer.instance->createWaylandSurfaceKHRUnique(create_info);
//}

idisplay *WaylandWindow::p_display() { return p_display_; }

WaylandDisplay::~WaylandDisplay() { wl_display_disconnect(display); }

WaylandDisplay &WaylandDisplay::dispatch_events_pending()
{
    wl_display_dispatch_pending(display);
    return *this;
}

WaylandDisplay &WaylandDisplay::dispatch_events_wait()
{
    while (!break_wait)
    {
        wl_display_dispatch_pending(display);
    }
    break_wait = false;
    return *this;
}

WaylandDisplay &WaylandDisplay::dispatch_events_break()
{
    break_wait = true;
    return *this;
}

window WaylandDisplay::create_window(const window_init &init)
{
    //  std::unique_ptr<WaylandWindow> p_window{new WaylandWindow};
    //  p_window->init(*this);
    //  return std::move(p_window);
    return window{};
}
} // namespace lmpl
