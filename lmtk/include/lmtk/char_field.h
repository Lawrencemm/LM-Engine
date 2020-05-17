#pragma once

#include "component.h"
#include "text_editor.h"
#include "text_layout.h"

namespace lmtk
{
struct char_field_init;
class char_field : public component_interface
{
  public:
    explicit char_field(char_field_init const &init);

    bool handle(input_event const &event) override;

    widget_interface &add_to_frame(lmgl::iframe *frame) override;

    lm::size2i get_size() override;
    lm::point2i get_position() override;

    widget_interface &set_rect(lm::point2i position, lm::size2i size) override;

    widget_interface &
      move_resources(lmgl::resource_sink &resource_sink) override;

    component_interface &update(
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink,
      lmtk::resource_cache const &resource_cache) override;

    [[nodiscard]] std::string get_value() const { return editor.text; }

    bool dirty{false};
    ifont const *font;
    text_layout layout;
    text_editor editor;
};

struct char_field_init
{
    lmgl::irenderer &renderer;
    lmtk::resource_cache const &resource_cache;
    std::array<float, 3> text_colour;
    lm::point2i position;
    std::string const &initial;
};
} // namespace lmtk