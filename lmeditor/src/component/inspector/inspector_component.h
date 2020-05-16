#pragma once

#include "../../controller/inspector/inspector_controller.h"
#include <entt/meta/meta.hpp>
#include <lmtk/shapes.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
class inspector_component : public inspector_interface
{
  public:
    explicit inspector_component(inspector_init const &init);

    bool handle(const lmtk::input_event &input_event) override;

    component_interface &update(
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink) override;

    component_interface &add_to_frame(lmgl::iframe *frame) override;

    void update(
      entt::registry &registry,
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink);

    void display(
      entt::registry const &registry,
      entt::entity entity,
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink);

    void clear(lmgl::irenderer *renderer, lmgl::resource_sink &resource_sink);

    lmtk::component_interface &
      move_resources(lmgl::resource_sink &sink) override;

    void create_text(lmgl::irenderer *renderer);
    void
      clear_text(lmgl::irenderer *renderer, lmgl::resource_sink &resource_sink);

    inspector_controller controller;
    lm::point2i position;
    lm::size2i size;
    lmgl::material text_material, background_material;
    lmtk::ifont const *font;
    lmtk::rect selection_background;

    std::vector<lmtk::text_layout> lines;

    lm::point2i get_position();

    void update_selection_background();
    std::string
      format_component_data(const std::string &name, const std::string &repr);
    inspector_component &set_rect(lm::point2i position, lm::size2i size);
    lm::size2i get_size();
    std::vector<command_description> get_command_descriptions();
    void display(const entt::registry &registry, entt::entity entity) override;
    void update(entt::registry &registry) override;
    void clear() override;
};
} // namespace lmeditor
