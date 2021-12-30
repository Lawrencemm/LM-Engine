#pragma once

#include "../../controller/map_editor/map_editor_controller.h"
#include <chrono>
#include <entt/entt.hpp>
#include <filesystem>
#include <lmeditor/component/command_palette.h>
#include <lmeditor/component/map_editor.h>
#include <lmgl/lmgl.h>
#include <lmhuv.h>
#include <lmlib/variant_visitor.h>
#include <lmng/physics.h>
#include <lmng/shapes.h>
#include <lmng/transform.h>
#include <lmtk/event.h>
#include <lmtk/shapes.h>
#include <lmtk/text_editor.h>
#include <lmtk/text_layout.h>
#include <range/v3/action/insert.hpp>

namespace lmeditor
{
class map_editor_component : public component_interface
{
  public:
    explicit map_editor_component(map_editor_init const &init);
    map_editor_component(map_editor_component const &) = delete;
    map_editor_component(map_editor_component &&) = delete;

    lmtk::component_interface &
      move_resources(lmgl::resource_sink &resource_sink) override;

    map_editor_component &set_rect(lm::point2i pos, lm::size2i size) override;
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    std::vector<command_description> get_command_descriptions() override;

  private:
    bool handle(const lmtk::event &event) override;

  private:
    void set_state_text(
      lmgl::irenderer *renderer,
      std::string new_text,
      lmgl::resource_sink &resource_sink,
      lmtk::resource_cache const &resource_cache);

    void render_state_text(lmgl::iframe *frame);

    void render_selection_outline(
      lmgl::iframe *frame,
      entt::registry const &map) const;

    lmgl::geometry create_box_geometry(
      lmgl::irenderer *renderer,
      lmgl::material box_material,
      lmgl::ibuffer *ubuffer);

    lmgl::material create_selection_stencil_material(lmgl::irenderer *renderer);
    lmgl::buffer create_selection_ubuffer(lmgl::irenderer *renderer);
    lmgl::material create_outline_material(lmgl::irenderer *renderer);

    map_editor_controller controller;
    lmhuv::pvisual_view visual_view;
    lmgl::material selection_stencil_material, selection_outline_material;
    lmgl::buffer box_vpositions, box_vnormals, box_indices;
    lmgl::buffer selection_outline_ubuffer;
    lmgl::geometry selection_stencil_geometry, selection_outline_geometry;
    uint32_t n_box_indices;
    lm::point2i position;
    lm::size2i size;

    Eigen::Vector3f light_direction;
    lm::point2i state_text_position{35, 20};
    std::array<float, 3> state_text_colour{1.f, 1.f, 1.f};
    lmtk::text_layout state_text_layout;

    std::array<float, 3> selection_outline_colour;
};
} // namespace lmeditor
