#include "xcb_window.h"
#include "xcb_display.h"

namespace lmpl
{
XcbWindow::XcbWindow(const window_init &init, XcbDisplay *display)
    : xcb_api_window{0}, p_display_{display}
{
    lm::size2u size = {unsigned(init.size.width), unsigned(init.size.height)};
    uint32_t mask = XCB_CW_EVENT_MASK;
    uint32_t values = {
      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS |
      XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS |
      XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
      XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_ENTER_WINDOW |
      XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
      XCB_EVENT_MASK_FOCUS_CHANGE};
    xcb_api_window = xcb_generate_id(p_display_->xcb_connection);
    xcb_create_window(
      p_display_->xcb_connection,
      XCB_COPY_FROM_PARENT,
      xcb_api_window,
      p_display_->xcb_api_screen->root,
      0,
      0,
      size.width,
      size.height,
      10,
      XCB_WINDOW_CLASS_INPUT_OUTPUT,
      p_display_->xcb_api_screen->root_visual,
      mask,
      &values);

    p_display_->context_map[xcb_api_window] = this;

    xcb_change_property(
      p_display_->xcb_connection,
      XCB_PROP_MODE_REPLACE,
      xcb_api_window,
      p_display_->wm_protocols_atom,
      XCB_ATOM_ATOM,
      32,
      1,
      &p_display_->wm_delete_window_atom);

    p_display_->flush();
}

XcbWindow &XcbWindow::show()
{
    xcb_map_window(p_display_->xcb_connection, xcb_api_window);
    p_display_->flush();
    return *this;
}

XcbWindow::~XcbWindow()
{
    if (p_display_)
    {
        xcb_destroy_window(p_display_->xcb_connection, xcb_api_window);
        p_display_->flush();
    }
}

idisplay *XcbWindow::p_display() { return p_display_; }

iwindow &XcbWindow::set_size(int width, int height)
{
    lm::size2u size = {(unsigned)width, (unsigned)height};
    xcb_configure_window(
      p_display_->xcb_connection,
      xcb_api_window,
      XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
      &size);
    p_display_->flush();
    return *this;
}

screen XcbWindow::get_screen()
{
    return p_display_->get_screen(xcb_api_window);
}

float XcbWindow::dpi_scale() { return (float)p_display_->dpi_scale_; }

lm::size2i XcbWindow::get_size_client()
{
    xcb_get_geometry_cookie_t cookie;
    xcb_get_geometry_reply_t *reply;

    cookie = xcb_get_geometry(p_display_->xcb_connection, xcb_api_window);
    /* ... do other work here if possible ... */
    if (!(reply =
           xcb_get_geometry_reply(p_display_->xcb_connection, cookie, NULL)))
    {
        throw std::runtime_error{"xcb_get_geometry_reply returned nullptr"};
    }
    auto size = lm::size2i(reply->width, reply->height);

    free(reply);

    return size;
}

xcb_connection_t *XcbWindow::get_connection()
{
    return p_display_->xcb_connection;
}

xcb_window_t XcbWindow::get_window() { return xcb_api_window; }
} // namespace lmpl
