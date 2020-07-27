#include "visual_registry_editor.h"
#include <fmt/format.h>
#include <lmhuv/box.h>
#include <lmlib/enumerate.h>
#include <lmlib/variant_visitor.h>
#include <lmng/meta/any_component.h>
#include <lmng/transform.h>

namespace lmeditor
{
component visual_registry_editor_init::operator()()
{
    return std::make_unique<visual_registry_editor>(*this);
}

visual_registry_editor::visual_registry_editor(
  visual_registry_editor_init const &init)
    : controller{init.registry, init.camera_init},
      visual_view{
        lmhuv::create_visual_view(lmhuv::visual_view_init{
          .registry = init.registry,
          .renderer = init.renderer,
          .render_box_colliders = true,
        }),
      },
      selection_stencil_material{
        create_selection_stencil_material(init.renderer)},
      selection_outline_material{create_outline_material(init.renderer)},
      selection_outline_ubuffer{create_selection_ubuffer(init.renderer)},
      light_direction{Eigen::Vector3f{0.2f, 0.6f, -0.75f}.normalized()},
      state_text_layout{lmtk::text_layout_init{
        .renderer = *init.renderer,
        .resource_cache = init.resource_cache,
        .colour = state_text_colour,
        .position = init.position + state_text_position,
        .text = visual_registry_controller::select_state::label,
      }},
      selection_outline_colour{init.selection_outline_colour},
      position{init.position},
      size{init.size}
{
    std::tie(box_vpositions, box_vnormals, box_indices, n_box_indices) =
      lmhuv::create_box_buffers(init.renderer);

    selection_stencil_geometry = create_box_geometry(
      init.renderer,
      selection_stencil_material,
      selection_outline_ubuffer.get());

    selection_outline_geometry = create_box_geometry(
      init.renderer,
      selection_outline_material,
      selection_outline_ubuffer.get());

    selection_stencil_geometry->set_n_indices(n_box_indices);
    selection_outline_geometry->set_n_indices(n_box_indices);
    selection_outline_geometry->set_line_width(6.f);
}

lmgl::geometry visual_registry_editor::create_box_geometry(
  lmgl::irenderer *renderer,
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

void visual_registry_editor::set_state_text(
  lmgl::irenderer *renderer,
  std::string new_text,
  lmgl::resource_sink &resource_sink,
  lmtk::resource_cache const &resource_cache)
{
    state_text_layout.set_text(
      *renderer, resource_cache.body_font.get(), new_text, resource_sink);
}

component_interface &visual_registry_editor::update(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink,
  lmtk::resource_cache const &resource_cache,
  lmtk::input_state const &input_state)
{
    set_state_text(
      renderer, controller.state_text, resource_sink, resource_cache);
    visual_view->set_camera_override(controller.camera);
    visual_view->update(*controller.map, renderer, resource_sink);
    return *this;
}

bool visual_registry_editor::add_to_frame(lmgl::iframe *frame)
{
    visual_view->add_to_frame(
      *controller.map, frame, lmgl::viewport{position, size});
    render_selection_outline(frame, *controller.map);
    render_state_text(frame);
    return false;
}

lmtk::widget_interface &
  visual_registry_editor::move_resources(lmgl::resource_sink &resource_sink)
{
    resource_sink.add(
      selection_stencil_material,
      selection_outline_material,
      box_vpositions,
      box_vnormals,
      box_indices,
      selection_outline_ubuffer,
      selection_stencil_geometry,
      selection_outline_geometry);
    visual_view->move_resources(resource_sink);
    state_text_layout.move_resources(resource_sink);
    return *this;
}

visual_registry_editor &
  visual_registry_editor::set_rect(lm::point2i pos, lm::size2i size)
{
    position = pos;
    this->size = size;
    controller.camera.aspect = (float)size.width / (float)size.height;
    state_text_layout.set_position(position + state_text_position);
    return *this;
}

lm::size2i visual_registry_editor::get_size() { return size; }

lm::point2i visual_registry_editor::get_position() { return position; }

bool visual_registry_editor::handle(const lmtk::input_event &input_event)
{
    return controller.handle(input_event);
}
std::vector<command_description>
  visual_registry_editor::get_command_descriptions()
{
    return controller.get_command_descriptions();
}
} // namespace lmeditor
