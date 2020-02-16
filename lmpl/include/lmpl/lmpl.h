#pragma once

#ifdef _WIN64
#include <windows.h>
namespace lmpl
{
inline void sleep(unsigned ms) { Sleep(ms); }
} // namespace lmpl

#elif defined(__linux__)
#include <unistd.h>

namespace lmpl
{
inline void sleep(unsigned ms) { usleep(ms * 1000); }
} // namespace lmpl

#endif

#include <functional>
#include <memory>
#include <variant>

#include <lmlib/geometry.h>
#include <lmlib/reference.h>

#include "keyboard.h"
#include "mouse.h"

namespace lmpl
{

class idisplay;
class iwindow;

struct window_message_base
{
    iwindow *window;
};
struct close_message : public window_message_base
{
};
struct mouse_button_down_message : public window_message_base
{
    mouse_button button;
    lm::point2i pos;
};
struct mouse_button_up_message : public window_message_base
{
    mouse_button button;
    lm::point2i pos;
};
struct mouse_move_message : public window_message_base
{
    lm::point2i pos;
};
struct key_down_message : public window_message_base
{
    key_code key;
};
struct key_up_message : public window_message_base
{
    key_code key;
};
struct repaint_message : public window_message_base
{
};
struct gain_focus_message : public window_message_base
{
};
struct lose_focus_message : public window_message_base
{
};

using window_message = std::variant<
  close_message,
  mouse_button_down_message,
  mouse_button_up_message,
  mouse_move_message,
  key_down_message,
  key_up_message,
  repaint_message,
  gain_focus_message,
  lose_focus_message>;

class iscreen
{
  public:
    virtual lm::size2i get_size() = 0;
};

using screen = lm::reference<iscreen>;

class iwindow
{
  public:
    virtual iwindow &show() = 0;
    virtual lm::size2i get_size_client() = 0;
    virtual iwindow &set_size(int width, int height) = 0;
    virtual screen get_screen() = 0;
    virtual float dpi_scale() = 0;
    virtual idisplay *p_display() = 0;
    virtual ~iwindow() = default;
};

struct window_init
{
    lm::size2i size;
};

typedef lm::reference<iwindow> window;

class idisplay
{
  public:
    virtual window_message wait_for_message() = 0;
    /// If get_message is currently blocking, make it return.
    virtual idisplay &break_wait() = 0;
    virtual window create_window(const window_init &init) = 0;
    virtual idisplay &show_cursor(bool) = 0;
    virtual idisplay &confine_cursor(
      iwindow &window,
      int from_x,
      int from_y,
      int to_x,
      int to_y) = 0;
    virtual idisplay &free_cursor() = 0;
    virtual screen get_primary_screen() = 0;
    virtual ~idisplay() = default;
};

typedef lm::reference<idisplay> display;

display create_display();

} // namespace lmpl
