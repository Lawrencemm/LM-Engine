#pragma once

namespace lmpl
{
enum class mouse_button
{
    L,
    R,
    M,
    U,
    D,
    n_mouse_buttons
};

class mouse_state
{
    bool map[(size_t)mouse_button::n_mouse_buttons] = {false};

  public:
    bool operator[](mouse_button button) const { return map[(unsigned)button]; }
    bool &operator[](mouse_button button) { return map[(unsigned)button]; }
};
}
