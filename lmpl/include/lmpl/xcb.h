#pragma once

#include <xcb/xcb.h>

#include <lmpl/lmpl.h>

namespace lmpl
{
class ixcb_window : public iwindow
{
  public:
    virtual xcb_connection_t *get_connection() = 0;
    virtual xcb_window_t get_window() = 0;
};
} // namespace lmpl
