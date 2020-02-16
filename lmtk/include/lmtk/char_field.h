#pragma once

#include "iwidget.h"
#include "text_editor.h"
#include "text_layout.h"

namespace lmtk
{
struct char_field_init;
class char_field : public iwidget
{
  public:
    explicit char_field(char_field_init const &init);

    std::string get_value() { return editor.text; }

    bool handle(
      input_event const &event,
      lmgl::irenderer *renderer,
      ifont const *font,
      resource_sink &resource_sink);
    iwidget &add_to_frame(lmgl::iframe *frame) override;
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    char_field &set_position(lm::point2i new_position);
    iwidget &set_rect(lm::point2i position, lm::size2i size) override;
    iwidget &move_resources(
      lmgl::irenderer *renderer,
      resource_sink &resource_sink) override;

    text_layout layout;
    text_editor editor;
};

struct char_field_init
{
    lmgl::irenderer &renderer;
    lmgl::material material;
    ifont const *font;
    std::array<float, 3> text_colour{0.f, 0.f, 0.f};
    lm::point2i position{0, 0};
    std::string const &initial;
};
} // namespace lmtk