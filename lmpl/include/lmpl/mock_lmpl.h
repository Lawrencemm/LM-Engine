#pragma once

#include <lmlib/pc_platform/create_display.h>

namespace lmpl
{

class MockDisplay : public idisplay
{
  public:
    window create_window(const window_init &init) override;
    idisplay &show_cursor(bool) override { return *this; };

    idisplay &free_cursor() override { return *this; };
    ~MockDisplay() override = default;

    idisplay &confine_cursor(
      iwindow &window,
      int from_x,
      int from_y,
      int to_x,
      int to_y) override;
    screen get_primary_screen() override;

  public:
};

class MockWindow : public virtual iwindow
{
  public:
    explicit MockWindow(MockDisplay *display) : display{display} {}

    iwindow &show() override { return *this; }

    size2i get_size_client() override { return size; }

    iwindow &set_size(int width, int height) override
    {
        size = {width, height};
        return *this;
    }

    screen get_screen() override;

    float dpi_scale() override { return 1.f; }

    idisplay *p_display() override { return display; }

    ~MockWindow() override = default;

    MockDisplay *display;
    size2i size;
};

class MockScreen : public iscreen
{
  public:
    size2i get_size() override { return {800, 600}; };
};
} // namespace lmpl
