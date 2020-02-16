#include "view.h"

#include <fmt/format.h>

#include <lmengine/reflection.h>
#include <lmengine/transform.h>
#include <lmlib/enumerate.h>
#include <lmlib/variant_visitor.h>

#include <lmhuv/box.h>

namespace lmeditor
{
map_editor_view::map_editor_view(init const &init)
    : renderer{init.renderer},
      visual_view{
        lmhuv::create_visual_view(lmhuv::visual_view_init{
          .registry = init.map,
          .renderer = init.renderer,
          .render_box_colliders = true,
        }),
      },
      text_material{init.text_material},
      selection_stencil_material{create_selection_stencil_material()},
      selection_outline_material{create_outline_material()},
      selection_outline_ubuffer{create_selection_ubuffer()},
      font{init.font},
      light_direction{Eigen::Vector3f{0.2f, 0.6f, -0.75f}.normalized()},
      state_text_layout{lmtk::text_layout_init{
        .renderer = *renderer,
        .material = text_material,
        .font = init.font,
        .colour = state_text_colour,
        .position = init.position + state_text_position,
        .text = init.initial_state_text,
      }},
      selection_outline_colour{init.selection_outline_colour},
      position{init.position},
      size{init.size}
{
    std::tie(box_vpositions, box_vnormals, box_indices, n_box_indices) =
      lmhuv::create_box_buffers(renderer);

    selection_stencil_geometry = create_box_geometry(
      selection_stencil_material, selection_outline_ubuffer.get());

    selection_outline_geometry = create_box_geometry(
      selection_outline_material, selection_outline_ubuffer.get());

    selection_stencil_geometry->set_n_indices(n_box_indices);
    selection_outline_geometry->set_n_indices(n_box_indices);
    selection_outline_geometry->set_line_width(6.f);
}

void map_editor_view::render(lmgl::iframe *frame, map_editor_model &model)
{
    visual_view->set_camera_override(model.camera);
    visual_view->add_to_frame(model.map, frame, lmgl::viewport{position, size});
    render_selection_outline(frame, model);
    render_state_text(frame);
}

void map_editor_view::add_box(entt::entity entity, entt::registry &entities)
{
    visual_view->add_box(renderer, entity);
}

lmgl::geometry map_editor_view::create_box_geometry(
  lmgl::material box_material,
  lmgl::ibuffer *ubuffer)
{
    auto geometry = renderer->create_geometry(lmgl::geometry_init{
      .material = box_material,
      .vertex_buffers =
        {
          box_vpositions.get(),
          box_vnormals.get(),
        },
      .index_buffer = box_indices.get(),
      .uniform_buffer = ubuffer,
    });
    return geometry;
}

void map_editor_view::move_resources(lmtk::resource_sink &sink)
{
    sink.add(
      renderer,
      selection_stencil_material,
      selection_outline_material,
      box_vpositions,
      box_vnormals,
      box_indices,
      selection_outline_ubuffer,
      selection_stencil_geometry,
      selection_outline_geometry);
    visual_view->move_resources(sink, renderer);
}

void map_editor_view::set_state_text(
  std::string new_text,
  lmtk::resource_sink &resource_sink)
{
    state_text_layout.set_text(*renderer, font, new_text, resource_sink);
}
} // namespace lmeditor
