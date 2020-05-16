#include "editable_mesh.h"

#include <lmgl/lmgl.h>
#include <lmlib/array_size.h>

#include <Resource.h>
#include <lmlib/concat.h>

LOAD_RESOURCE(
  uniform_colour_vshader_data,
  shaders_editable_mesh_uniform_colour_vert_spv);
LOAD_RESOURCE(colour_vshader, shaders_editable_mesh_colour_vert_spv);
LOAD_RESOURCE(frag_shader, shaders_editable_mesh_flat_vertex_frag_spv);

struct mesh_ubuffer_data
{
    std::array<float, 4> fill_colour;
    Eigen::Matrix4f transform;
};

struct edge_ubuffer_data
{
    Eigen::Matrix4f transform;
    float point_size;
};

struct points_ubuffer_data
{
    Eigen::Matrix4f transform;
    float point_size;
};

namespace lmeditor
{
void editable_mesh::init_rendering(const editable_mesh_init &init)
{
    init_uniform_buffers(init);
    init_vertex_buffers();
    init_mesh_index_buffers();
    init_common_mesh_properties();
    init_tri_mesh();
    init_lines_meshes(init);
    init_points_mesh(init);
}

void editable_mesh::init_common_mesh_properties()
{
    vpositions_property =
      lmgl::geometry_property{.input_type = lmgl::geometry_input_type::float_3,
                              .buffer = vpositions_buffer.get()};
    vcolours_property =
      lmgl::geometry_property{.input_type = lmgl::geometry_input_type::float_3,
                              .buffer = vcolours_buffer.get()};
    vpositions_edge_property =
      lmgl::geometry_property{.input_type = lmgl::geometry_input_type::float_3,
                              .buffer = vpositions_edge_buffer.get()};
    vcolours_edge_property =
      lmgl::geometry_property{.input_type = lmgl::geometry_input_type::float_3,
                              .buffer = vcolours_edge_buffer.get()};
}

void editable_mesh::init_vertex_buffers()
{
    vpositions_buffer = renderer->create_buffer(
      lmgl::buffer_init{.usage = lmgl::render_buffer_vertex_usage,
                        .data = geo_mesh.points_data()});
    vcolours_buffer = renderer->create_buffer(
      lmgl::buffer_init{.usage = lmgl::render_buffer_vertex_usage,
                        .data = geo_mesh.colors_data()});

    vpositions_edge_buffer = renderer->create_buffer(
      lmgl::buffer_init{.usage = lmgl::render_buffer_vertex_usage,
                        .data = lm::raw_array_proxy(edge_mesh_positions)});
    vcolours_edge_buffer = renderer->create_buffer(
      lmgl::buffer_init{.usage = lmgl::render_buffer_vertex_usage,
                        .data = lm::raw_array_proxy(edge_mesh_colours)});
}

void editable_mesh::init_uniform_buffers(const editable_mesh_init &init)
{
    auto mesh_init =
      mesh_ubuffer_data{lm::concat(init.solid_colour, 1.f), Eigen::Matrix4f{}};

    tris_ubuffer = renderer->create_buffer(lmgl::buffer_init{
      .usage = lmgl::render_buffer_uniform_usage,
      .data = lm::array_proxy<char const>(
        (char const *)&mesh_init, sizeof(mesh_init)),
    });

    edge_ubuffer = renderer->create_buffer(lmgl::buffer_init{
      .usage = lmgl::render_buffer_uniform_usage,
      .data = lm::array_proxy<char const>(nullptr, sizeof(edge_ubuffer_data)),
    });

    auto points_init = points_ubuffer_data{Eigen::Matrix4f{}, init.point_size};
    points_ubuffer = renderer->create_buffer(lmgl::buffer_init{
      .usage = lmgl::render_buffer_uniform_usage,
      .data = lm::array_proxy<char const>(
        (char const *)&points_init, sizeof(points_init)),
    });
}

void editable_mesh::init_mesh_index_buffers()
{
    mesh_index_buffer = renderer->create_buffer(
      lmgl::buffer_init{.usage = lmgl::render_buffer_index_usage,
                        .data = lm::raw_array_proxy(mesh_indices)});
    edge_index_buffer = renderer->create_buffer(
      lmgl::buffer_init{.usage = lmgl::render_buffer_index_usage,
                        .data = lm::raw_array_proxy(edge_indices)});
}

void editable_mesh::init_lines_meshes(const editable_mesh_init &init)
{
    lmgl::geometry_init glines_mesh_init{
      .topology = lmgl::geometry_topology::line_list,
      .line_width = init.line_width,
      .vshader_data = lm::raw_array_proxy(colour_vshader),
      .pshader_data = lm::raw_array_proxy(frag_shader),
      .vertex_properties = {vpositions_edge_property, vcolours_edge_property},
      .index_buffer = edge_index_buffer.get(),
      .index_type = get_mesh_index_type(),
      .uniform_buffer = edge_ubuffer.get(),
      .do_depth_test = false};

    lines_render_geometry = renderer->create_geometry(glines_mesh_init);
    lines_render_geometry->set_n_indices(edge_indices.size());
}

void editable_mesh::init_points_mesh(const editable_mesh_init &init)
{

    lmgl::geometry_init gpoints_mesh_init{
      .topology = lmgl::geometry_topology::point_list,
      .point_size = init.point_size,
      .vshader_data = lm::raw_array_proxy(colour_vshader),
      .pshader_data = lm::raw_array_proxy(frag_shader),
      .vertex_properties = {vpositions_property, vcolours_property},
      .index_buffer = mesh_index_buffer.get(),
      .index_type = get_mesh_index_type(),
      .uniform_buffer = points_ubuffer.get(),
      .do_depth_test = false};

    points_render_geometry = renderer->create_geometry(gpoints_mesh_init);
    points_render_geometry->set_n_indices(mesh_indices.size());
}

void editable_mesh::init_tri_mesh()
{
    lmgl::geometry_init gmesh_init{
      .topology = lmgl::geometry_topology::triangle_list,
      .vshader_data = lm::raw_array_proxy(uniform_colour_vshader_data),
      .pshader_data = lm::raw_array_proxy(frag_shader),
      .vertex_properties = {vpositions_property},
      .index_buffer = mesh_index_buffer.get(),
      .index_type = get_mesh_index_type(),
      .uniform_buffer = tris_ubuffer.get(),
      .do_depth_test = true};

    mesh_render_geometry = renderer->create_geometry(gmesh_init);
    mesh_render_geometry->set_n_indices(mesh_indices.size());
}

editable_mesh &editable_mesh::set_colour(
  editable_mesh::vertex_handle vh,
  std::array<float, 3> colour)
{
    geo_mesh.set_color(vh, Eigen::Vector3f{colour.data()});
    return *this;
}

editable_mesh &
  editable_mesh::set_colour(edge_handle eh, std::array<float, 3> colour)
{
    geo_mesh.set_color(eh, Eigen::Vector3f{colour.data()});
    return *this;
}

void editable_mesh::on_complete(lmgl::iframe *frame)
{
    frame_resources.erase(frame);
}

void editable_mesh::render(lmgl::iframe *frame, const lm::camera &camera)
{
    frame_resources[frame] = std::array{
      std::move(vpositions_buffer),
      std::move(vcolours_buffer),
      std::move(vpositions_edge_buffer),
      std::move(vcolours_edge_buffer),
      std::move(mesh_index_buffer),
      std::move(edge_index_buffer),
    };

    vpositions_buffer = renderer->create_buffer(lmgl::buffer_init{
      lmgl::render_buffer_vertex_usage,
      geo_mesh.points_data(),
    });
    vcolours_buffer = renderer->create_buffer(lmgl::buffer_init{
      lmgl::render_buffer_vertex_usage,
      geo_mesh.colors_data(),
    });

    mesh_render_geometry->set_vbuffers({vpositions_buffer.get()}, 0);
    points_render_geometry->set_vbuffers(
      {vpositions_buffer.get(), vcolours_buffer.get()}, 0);

    regenerate_mesh_rendering_data();

    vpositions_edge_buffer = renderer->create_buffer(lmgl::buffer_init{
      lmgl::render_buffer_vertex_usage,
      lm::raw_array_proxy(edge_mesh_positions),
    });
    vcolours_edge_buffer = renderer->create_buffer(lmgl::buffer_init{
      lmgl::render_buffer_vertex_usage,
      lm::raw_array_proxy(edge_mesh_colours),
    });

    lines_render_geometry->set_vbuffers(
      {vpositions_edge_buffer.get(), vcolours_edge_buffer.get()}, 0);

    mesh_index_buffer = renderer->create_buffer(lmgl::buffer_init{
      lmgl::render_buffer_index_usage,
      lm::raw_array_proxy(mesh_indices),
    });
    edge_index_buffer = renderer->create_buffer(lmgl::buffer_init{
      lmgl::render_buffer_index_usage,
      lm::raw_array_proxy(edge_indices),
    });

    points_render_geometry->set_index_buffer(mesh_index_buffer.get());
    mesh_render_geometry->set_index_buffer(mesh_index_buffer.get());
    lines_render_geometry->set_index_buffer(edge_index_buffer.get());

    points_render_geometry->set_n_indices(mesh_indices.size());
    mesh_render_geometry->set_n_indices(mesh_indices.size());
    lines_render_geometry->set_n_indices(edge_indices.size());

    auto transform = camera.view_matrix();

    lmgl::add_buffer_update(frame, tris_ubuffer.get(), transform, 16);
    lmgl::add_buffer_update(frame, edge_ubuffer.get(), transform);
    lmgl::add_buffer_update(frame, points_ubuffer.get(), transform);

    frame->add({
      mesh_render_geometry.get(),
      lines_render_geometry.get(),
      points_render_geometry.get(),
    });
}
} // namespace lmeditor
