#include "../../trimesh/util.h"
#include "view.h"

#include <Resource.h>
#include <lmhuv/box.h>

LOAD_RESOURCE(stencil_vshader_data, shaders_stencil_vert_spv);
LOAD_RESOURCE(outline_pshader_data, shaders_selection_outline_frag_spv);

struct selection_ubuffer_data
{
    Eigen::Matrix4f mvp;
    std::array<float, 3> colour;
};

namespace lmeditor
{
lmgl::buffer map_editor_view::create_selection_ubuffer()
{
    return renderer->create_buffer(lmgl::buffer_init{
      .usage = lmgl::render_buffer_usage::uniform,
      .data = {nullptr, sizeof(selection_ubuffer_data)},
    });
}

lmgl::material map_editor_view::create_selection_stencil_material()
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

lmgl::material map_editor_view::create_outline_material()
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

void map_editor_view::render_selection_outline(
  lmgl::iframe *frame,
  map_editor_model const &model) const
{
    if (model.selected_box == entt::null)
        return;

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
    auto const &transform =
      lmng::resolve_transform(model.map, model.selected_box);

    lmgl::add_buffer_update(
      frame,
      selection_outline_ubuffer.get(),
      selection_ubuffer_data{
        lmhuv::get_box_mvp_matrix(
          transform, model.get_selection_extents(), model.camera),
        selection_outline_colour,
      });
    frame->add({selection_stencil_geometry.get()}, viewport);
    frame->add({selection_outline_geometry.get()}, viewport);
}

void map_editor_view::render_state_text(lmgl::iframe *frame)
{
    state_text_layout.render(frame);
}
} // namespace lmeditor
