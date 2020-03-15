#pragma once

#include <string>
#include <vector>

#include <lmgl/lmgl.h>

#include "lmtk.h"
#include "shapes.h"
#include "text_layout.h"

namespace lmtk
{
struct text_line_selector_init;

class text_line_selector : public iwidget
{
  public:
    explicit text_line_selector(text_line_selector_init const &init);

    iwidget &add_to_frame(lmgl::iframe *frame);
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    iwidget &set_rect(lm::point2i position, lm::size2i size) override;
    iwidget &move_resources(
      lmgl::irenderer *renderer,
      resource_sink &resource_sink) override;

    bool handle(lmtk::input_event const &input_event);
    int get_selection_index() { return selection_index; }

  private:
    int selection_index;
    std::vector<lmtk::text_layout> line_layouts;
    lmtk::rect selection_background;
};

struct text_line_selector_init
{
    std::vector<std::string> const &lines;
    lmgl::irenderer *renderer;
    lmgl::material font_material;
    ifont const *font;
    lmgl::material rect_material;

    text_line_selector operator()() { return text_line_selector{*this}; }
};
} // namespace lmtk
