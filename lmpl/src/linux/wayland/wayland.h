
#pragma once

#include <wayland-client.h>

#include <lmlib/geometry.h>

#include <lmpl/lmpl.h>

namespace lmpl
{
class vulkan_renderer;
}

namespace lmpl
{

class WaylandDisplay;

class WaylandWindow : public iwindow
{
  public:
    using Size = lm::size2<unsigned>;

    WaylandWindow &init(WaylandDisplay &display);
    WaylandWindow &show();

    WaylandWindow &size(Size new_size);
    //  Size size() override { return desired_size; }
    //  unsigned width() { return desired_size.x; }
    //  unsigned height() { return desired_size.y; }

    idisplay *p_display();

  public:
    wl_surface *surface;
    WaylandDisplay *p_display_;
    wl_shell_surface *shell_surface;
    Size desired_size{800, 600};

    static void handle_configure(
      void *data, wl_shell_surface *shell_surface, uint32_t edges,
      int32_t width, int32_t height);
};

class WaylandDisplay : public idisplay
{
  public:
    WaylandDisplay();
    WaylandDisplay &init();
    WaylandDisplay &dispatch_events_pending();
    WaylandDisplay &dispatch_events_wait();
    WaylandDisplay &dispatch_events_break();

    window create_window(const window_init &init);

    ~WaylandDisplay();

  public:
    wl_display *display;
    wl_registry *registry;
    wl_compositor *compositor;
    wl_shell *shell;
    wl_seat *seat;

  private:
    bool break_wait{false};
};

class WaylandException : public std::runtime_error
{
  public:
    WaylandException(std::string msg)
        : std::runtime_error{"Wayland error: " + msg}
    {
    }
};

class WaylandDisplayError : public WaylandException
{
  public:
    WaylandDisplayError()
        : WaylandException{"wl_display_connect returned NULL."}
    {
    }
};

class NoWaylandEnvVar : public WaylandException
{
  public:
    NoWaylandEnvVar()
        : WaylandException{"No WAYLAND_DISPLAY environment variable."}
    {
    }
};

class NoCompositor : public WaylandException
{
  public:
    NoCompositor() : WaylandException{"Compositor is NULL."} {}
};

class NoShell : WaylandException
{
  public:
    NoShell() : WaylandException{"Shell is NULL."} {}
};

class WindowSurfaceNull : WaylandException
{
  public:
    WindowSurfaceNull() : WaylandException{"GtkPlusWindow surface is NULL."} {}
};

class WindowShellSurfaceNull : WaylandException
{
  public:
    WindowShellSurfaceNull()
        : WaylandException{"GtkPlusWindow shell surface is NULL."}
    {
    }
};
} // namespace lmpl
