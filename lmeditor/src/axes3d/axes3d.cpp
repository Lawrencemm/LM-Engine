#include "axes3d.h"

#include <Resource.h>

#include <lmlib/array_size.h>

LOAD_RESOURCE(colour_vshader, shaders_editable_mesh_colour_vert_spv);
LOAD_RESOURCE(frag_shader, shaders_editable_mesh_flat_vertex_frag_spv);

struct ubuffer_data
{
    Eigen::Matrix4f transform;
};

namespace lmeditor
{

axes::axes(const axes_init &init) : renderer{init.renderer}
{
    init_geometry(init);
    init_rendering(init);
}

void axes::init_geometry(const axes_init &init)
{
    positions = std::array{std::array{-init.extent, 0.f, 0.f},
                           std::array{init.extent, 0.f, 0.f},
                           std::array{0.f, 0.f, -init.extent},
                           std::array{0.f, 0.f, init.extent}};

    colours = std::array{init.x_axis_colour,
                         init.x_axis_colour,
                         init.z_axis_colour,
                         init.z_axis_colour};

    index_data = {0, 1, 2, 3};
}

void axes::init_rendering(const axes_init &init)
{
    ubuffer = renderer->create_buffer(lmgl::buffer_init{
      lmgl::uniform,
      lm::array_proxy<char const>(nullptr, sizeof(Eigen::Matrix4f)),
    });

    points_vbuffer = renderer->create_buffer(
      lmgl::buffer_init{lmgl::vertex, lm::raw_array_proxy(positions)});

    colours_vbuffer = renderer->create_buffer(
      lmgl::buffer_init{lmgl::vertex, lm::raw_array_proxy(colours)});

    indices_render_buffer = renderer->create_buffer(
      lmgl::buffer_init{lmgl::index, lm::raw_array_proxy(index_data)});

    material = renderer->create_material(lmgl::material_init{
      .vshader_spirv = lm::raw_array_proxy(colour_vshader),
      .pshader_spirv = lm::raw_array_proxy(frag_shader),
      .index_type = lmgl::index_type::two_bytes,
      .topology = lmgl::topology::line_list,
      .vertex_input_types =
        {
          lmgl::input_type::float_3,
          lmgl::input_type::float_3,
        },
      .uniform_size = sizeof(Eigen::Matrix4f),
    });

    lmgl::geometry_init geo_init{
      .material = material,
      .vertex_buffers =
        {
          points_vbuffer.get(),
          colours_vbuffer.get(),
        },
      .index_buffer = indices_render_buffer.get(),
      .uniform_buffer = ubuffer.get(),
    };

    render_geometry = renderer->create_geometry(geo_init);
    render_geometry->set_n_indices(index_data.size());
}

void axes::on_render(lmgl::iframe *frame, lm::camera const &camera)
{
    lmgl::add_buffer_update(frame, ubuffer.get(), camera.view_matrix());
    frame->add({render_geometry.get()});
}
} // namespace lmeditor
