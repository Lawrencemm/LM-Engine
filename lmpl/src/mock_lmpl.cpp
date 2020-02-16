#include <lmlib/pc_platform/test/mock_lmpl.h>

namespace lmpl
{

window MockDisplay::create_window(const window_init &init)
{
    return std::make_unique<MockWindow>(this);
}

idisplay &MockDisplay::confine_cursor(
  iwindow &window,
  int from_x,
  int from_y,
  int to_x,
  int to_y)
{
    return *this;
}

screen MockDisplay::get_primary_screen() { return lmpl ::screen(); }

screen MockWindow::get_screen() { return std::make_unique<MockScreen>(); }
} // namespace lmpl
