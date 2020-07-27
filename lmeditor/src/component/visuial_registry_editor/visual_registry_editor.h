#pragma once

#include "../../controller/visual_registry_editor/visual_registry_controller.h"
#include <chrono>
#include <entt/entt.hpp>
#include <filesystem>
#include <lmeditor/component/command_help.h>
#include <lmeditor/component/visual_registry_editor.h>
#include <lmgl/lmgl.h>
#include <lmhuv.h>
#include <lmlib/variant_visitor.h>
#include <lmng/physics.h>
#include <lmng/shapes.h>
#include <lmng/transform.h>
#include <lmtk/input_event.h>
#include <lmtk/shapes.h>
#include <lmtk/text_editor.h>
#include <lmtk/text_layout.h>
#include <range/v3/action/insert.hpp>

namespace lmeditor
{
class visual_registry_editor : public component_interface
{
  public:
    explicit visual_registry_editor(visual_registry_editor_init const &init);
    visual_registry_editor(visual_registry_editor const &) = delete;
    visual_registry_editor(visual_registry_editor &&) = delete;

    component_interface &update(
      lmgl::irenderer *renderer,
      lmgl::resource_sink &resource_sink,
      lmtk::resource_cache const &resource_cache,
      lmtk::input_state const &input_state) override;

    bool add_to_frame(lmgl::iframe *frame) override;

    widget_interface &
      move_resources(lmgl::resource_sink &resource_sink) override;

    visual_registry_editor &set_rect(lm::point2i pos, lm::size2i size) override;
    lm::size2i get_size() override;
    lm::point2i get_position() override;
    std::vector<command_description> get_command_descriptions() override;

  private:
    bool handle(const lmtk::input_event &input_event) override;

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

    visual_registry_controller controller;
    lmhuv::pvisual_view visual_view;
    lmgl::material selection_stencil_material, selection_outline_material;
    lmgl::buffer box_vpositions, box_vnormals, box_indices;
    lmgl::buffer selection_outline_ubuffer;
    lmgl::geometry selection_stencil_geometry, selection_outline_geometry;
    size_t n_box_indices;
    lm::point2i position;
    lm::size2i size;

    Eigen::Vector3f light_direction;
    lm::point2i state_text_position{35, 20};
    std::array<float, 3> state_text_colour{1.f, 1.f, 1.f};
    lmtk::text_layout state_text_layout;

    std::array<float, 3> selection_outline_colour;
};
} // namespace lmeditor
