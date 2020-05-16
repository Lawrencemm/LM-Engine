#pragma once

#include "component.h"
#include "font.h"
#include "input_event.h"
#include "shapes.h"
#include "text_layout.h"
#include <entt/signal/sigh.hpp>
#include <lmgl/lmgl.h>
#include <lmlib/reference.h>
#include <string>
#include <vector>

namespace lmtk
{
struct choice_list_init;

class choice_list : public component_interface
{
  public:
    explicit choice_list(choice_list_init const &init);

    widget_interface &add_to_frame(lmgl::iframe *frame) override;
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    widget_interface &set_rect(lm::point2i position, lm::size2i size) override;

    widget_interface &move_resources(
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink) override;

    bool handle(lmtk::input_event const &input_event) override;

    component_interface &update(
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink) override;

    entt::sink<bool(unsigned, std::string const &)> on_selected();

  private:
    unsigned selection_index;
    std::vector<std::string> lines;
    std::vector<lmtk::text_layout> line_layouts;
    lmtk::rect selection_background;
    entt::sigh<bool(unsigned, std::string const &)> selected;
};

struct choice_list_init
{
    std::vector<std::string> const &choices;
    lmgl::irenderer *renderer;
    lmgl::material font_material;
    ifont const *font;
    lmgl::material rect_material;

    choice_list operator()();

    lm::reference<choice_list> unique();
};
} // namespace lmtk
