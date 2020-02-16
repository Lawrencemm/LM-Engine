#pragma once

#include "../../viewport/viewport.h"
#include "../model/model.h"

#include <chrono>
#include <filesystem>

#include <entt/entt.hpp>
#include <range/v3/action/insert.hpp>

#include <lmeditor/command_help.h>
#include <lmeditor/map_editor.h>
#include <lmengine/physics.h>
#include <lmengine/shapes.h>
#include <lmengine/transform.h>
#include <lmgl/lmgl.h>
#include <lmhuv.h>
#include <lmlib/variant_visitor.h>
#include <lmtk/lmtk.h>
#include <lmtk/shapes.h>
#include <lmtk/text_editor.h>
#include <lmtk/text_layout.h>

namespace lmeditor
{
class map_editor_view
{
  public:
    struct init;
    explicit map_editor_view(init const &init);

    void set_rect(lm::point2i pos, lm::size2i size)
    {
        position = pos;
        this->size = size;
        state_text_layout.set_position(position + state_text_position);
    }

    map_editor_view(map_editor_view const &) = delete;
    map_editor_view(map_editor_view &&) = delete;

    void render(lmgl::iframe *frame, map_editor_model &model);
    void move_resources(lmtk::resource_sink &sink);

  public:
    void render_state_text(lmgl::iframe *frame);
    void render_selection_outline(
      lmgl::iframe *frame,
      map_editor_model const &model) const;
    lmgl::material create_selection_stencil_material();

    lmgl::geometry
      create_box_geometry(lmgl::material box_material, lmgl::ibuffer *ubuffer);

    void add_box(entt::entity entity, entt::registry &entities);

    lmgl::buffer create_selection_ubuffer();
    lmgl::material create_outline_material();

    lmgl::irenderer *renderer;
    lmhuv::pvisual_view visual_view;
    lmgl::material text_material, selection_stencil_material,
      selection_outline_material;
    lmgl::buffer box_vpositions, box_vnormals, box_indices;
    lmgl::buffer selection_outline_ubuffer;
    lmgl::geometry selection_stencil_geometry, selection_outline_geometry;
    size_t n_box_indices;
    lmtk::ifont const *font;
    lm::point2i position;
    lm::size2i size;

    Eigen::Vector3f light_direction;
    lm::point2i state_text_position{35, 20};
    std::array<float, 3> state_text_colour{1.f, 1.f, 1.f};
    lmtk::text_layout state_text_layout;

    std::array<float, 3> selection_outline_colour;
    void
      set_state_text(std::string new_text, lmtk::resource_sink &resource_sink);
};

struct map_editor_view::init : public map_editor_init
{
    entt::registry &map;
    std::string initial_state_text;
};
} // namespace lmeditor
