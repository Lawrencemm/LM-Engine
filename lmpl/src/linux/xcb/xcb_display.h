#pragma once
#include <map>
#include <unordered_map>

#include <giomm/settings.h>

#include <lmpl/xcb.h>

#include <lmlib/geometry.h>

#include <lmpl/lmpl.h>

namespace lmpl
{

class XcbWindow;

class XcbDisplay : public idisplay
{
  public:
    idisplay &break_wait() override;

  public:
    XcbDisplay();
    ~XcbDisplay();
    XcbDisplay(XcbDisplay &) = delete;

    window create_window(const window_init &init) override;

    screen get_primary_screen() override;

    screen get_screen(xcb_window_t window);

  public:
    xcb_connection_t *xcb_connection;
    int screen_number;
    const xcb_setup_t *xcb_setup;
    xcb_screen_t *xcb_api_screen;

    xcb_atom_t wm_protocols_atom, wm_delete_window_atom, lmlib_quit_atom;

    std::map<xcb_window_t, XcbWindow *> context_map;

    std::optional<window_message> process_xcb_event(xcb_generic_event_t *event);

    void flush() const;
    window_message wait_for_message() override;
    idisplay &show_cursor(bool b) override;
    idisplay &confine_cursor(
      iwindow &window,
      int from_x,
      int from_y,
      int to_x,
      int to_y) override;
    idisplay &free_cursor() override;
    double dpi_scale_;
    Glib::RefPtr<Gio::Settings> text_scale_setting;

    xcb_window_t dummy_window;
};
} // namespace lmpl
