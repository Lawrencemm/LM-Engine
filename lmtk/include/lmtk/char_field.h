#pragma once

#include "component.h"
#include "text_editor.h"
#include "text_layout.h"
#include "shapes.h"

namespace lmtk
{
struct char_field_init;
class char_field : public component_interface
{
  public:
    explicit char_field(char_field_init const &init);

    lmtk::component_state handle(event const &event) override;

    lm::size2i get_size() override;
    lm::point2i get_position() override;

    component_interface &
      set_rect(lm::point2i position, lm::size2i size) override;

    component_interface &
      move_resources(lmgl::resource_sink &resource_sink) override;

    [[nodiscard]] std::string get_value() const { return editor.text; }

    float get_seconds_to_next_blink();

    bool dirty{false};
    ifont const *font;
    text_layout layout;
    rect cursor;
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