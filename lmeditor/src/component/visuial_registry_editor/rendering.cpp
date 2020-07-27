#include "../../model/trimesh/util.h"
#include "visual_registry_editor.h"

#include <Resource.h>
#include <lmeditor/model/selection.h>
#include <lmhuv/box.h>
#include <lmng/extents.h>

LOAD_RESOURCE(stencil_vshader_data, shaders_stencil_vert_spv);
LOAD_RESOURCE(outline_pshader_data, shaders_selection_outline_frag_spv);

struct selection_ubuffer_data
{
    Eigen::Matrix4f mvp;
    std::array<float, 3> colour;
};

namespace lmeditor
{
lmgl::buffer
  visual_registry_editor::create_selection_ubuffer(lmgl::irenderer *renderer)
{
    return renderer->create_buffer(lmgl::buffer_init{
      .usage = lmgl::render_buffer_usage::uniform,
      .data = {nullptr, sizeof(selection_ubuffer_data)},
    });
}

lmgl::material visual_registry_editor::create_selection_stencil_material(
  lmgl::irenderer *renderer)
{
    return renderer->create_material(lmgl::material_init{
      .vshader_spirv = lm::raw_array_proxy(stencil_vshader_data),
      .index_type = get_mesh_index_type(),
      .vertex_input_types =
        {
          lmgl::input_type::float_3,
          lmgl::input_type::float_3,
        },
      .uniform_size = sizeof(selection_ubuffer_data),
      .write_stencil = true,
    });
}

lmgl::material
  visual_registry_editor::create_outline_material(lmgl::irenderer *renderer)
{
    return renderer->create_material(lmgl::material_init{
      .vshader_spirv = lm::raw_array_proxy(stencil_vshader_data),
      .pshader_spirv = lm::raw_array_proxy(outline_pshader_data),
      .index_type = get_mesh_index_type(),
      .vertex_input_types =
        {
          lmgl::input_type::float_3,
          lmgl::input_type::float_3,
        },
      .uniform_size = sizeof(selection_ubuffer_data),
      .do_depth_test = false,
      .test_stencil = true,
      .polygon_mode = lmgl::polygon_mode::line,
    });
}

void visual_registry_editor::render_selection_outline(
  lmgl::iframe *frame,
  entt::registry const &map) const
{
    auto selection_view = controller.get_registry()->view<selected>();
    if (selection_view.empty())
        return;

    auto selection = selection_view.front();

    lm::point2f viewport_pos{
      static_cast<float>(position.x),
      static_cast<float>(position.y),
    };
    lm::size2f viewport_size{
      static_cast<float>(size.width),
      static_cast<float>(size.height),
    };
    auto viewport = lmgl::viewport{
      viewport_pos,
      viewport_size,
    };
    auto const &transform = lmng::resolve_transform(map, selection);

    lmgl::add_buffer_update(
      frame,
      selection_outline_ubuffer.get(),
      selection_ubuffer_data{
        lmhuv::get_box_mvp_matrix(
          transform,
          lmng::get_extents(map, selection),
          controller.get_camera()),
        selection_outline_colour,
      });
    frame->add({selection_stencil_geometry.get()}, viewport);
    frame->add({selection_outline_geometry.get()}, viewport);
}

void visual_registry_editor::render_state_text(lmgl::iframe *frame)
{
    state_text_layout.render(frame, position, size);
}
} // namespace lmeditor
