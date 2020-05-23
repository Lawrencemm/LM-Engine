#pragma once

#include "../../controller/entity_list/entity_list_controller.h"
#include <entt/fwd.hpp>
#include <lmeditor/component/entity_list.h>
#include <lmtk/component.h>
#include <lmtk/shapes.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
class entity_list_component : public component_interface
{
  public:
    explicit entity_list_component(entity_list_init const &init);

    lm::size2i get_size() override;
    lm::point2i get_position() override;
    bool add_to_frame(lmgl::iframe *frame) override;

    entity_list_component &
      set_rect(lm::point2i position, lm::size2i size) override;

    entity_list_component &
      move_resources(lmgl::resource_sink &resource_sink) override;

    void reset(
      lmgl::irenderer &renderer,
      lmgl::resource_sink &resource_sink,
      lmtk::resource_cache const &resource_cache,
      entt::registry const &registry);

    lmtk::text_layout &selected_line();
    void update_selection_background();
    bool handle(const lmtk::input_event &input_event) override;
    component_interface &update(
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink,
      lmtk::resource_cache const &resource_cache,
      lmtk::input_state const &input_state) override;
    std::vector<command_description> get_command_descriptions() override;

    entity_list_controller controller;
    lm::point2i position;
    lm::size2i size;
    lmtk::rect selection_background;
    std::vector<lmtk::text_layout> line_layouts;
};
} // namespace lmeditor
