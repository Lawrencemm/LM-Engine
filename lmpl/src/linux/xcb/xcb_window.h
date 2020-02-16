#pragma once

#include <lmpl/xcb.h>

#include <lmlib/geometry.h>

namespace lmpl
{

class XcbDisplay;

class XcbWindow : public ixcb_window
{
    friend class XcbDisplay;

  public:
    using Size = lm::size2<unsigned>;

    explicit XcbWindow(const window_init &init, XcbDisplay *display);
    XcbWindow &show() override;
    idisplay *p_display();

    ~XcbWindow() override;
    iwindow &set_size(int width, int height) override;

    screen get_screen() override;
    float dpi_scale() override;
    lm::size2i get_size_client() override;
    xcb_connection_t *get_connection() override;
    xcb_window_t get_window() override;

  public:
    xcb_window_t xcb_api_window;
    XcbDisplay *p_display_;
    Size size;
};
} // namespace lmpl
