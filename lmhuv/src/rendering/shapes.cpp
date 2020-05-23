#include "shapes.h"
#include "../trimesh/util.h"

#include <Resource.h>

#include <lmgl/lmgl.h>
#include <lmhuv/box.h>
#include <lmlib/concat.h>
#include <lmng/shapes.h>

LOAD_RESOURCE(box_vshader_data, shaders_box_vert_spv);
LOAD_RESOURCE(box_pshader_data, shaders_box_frag_spv);
LOAD_RESOURCE(box_flat_pshader_data, shaders_box_flat_frag_spv);

struct uniform_data
{
    std::array<float, 4> colour;
    Eigen::Matrix4f model;
    Eigen::Matrix4f mvp;
    Eigen::Vector3f camera_dir;
};

namespace lmhuv
{
lmgl::material box_material_init::operator()()
{
    return renderer.create_material(lmgl::material_init{
      .vshader_spirv = lm::raw_array_proxy(box_vshader_data),
      .pshader_spirv = lm::raw_array_proxy(box_pshader_data),
      .index_type = get_mesh_index_type(),
      .vertex_input_types =
        {
          lmgl::input_type::float_3,
          lmgl::input_type::float_3,
        },
      .uniform_size = sizeof(uniform_data),
      .write_stencil = write_stencil,
      .polygon_mode = polygon_mode,
    });
}

lmgl::buffer create_box_ubuffer(lmgl::irenderer *renderer)
{
    return renderer->create_buffer(lmgl::buffer_init{
      .usage = lmgl::render_buffer_usage::uniform,
      .data = lm::array_proxy<char const>(nullptr, sizeof(uniform_data)),
    });
}

void update_box_uniform(
  lmgl::iframe *frame,
  lmgl::ibuffer *buffer,
  lm::camera const &camera,
  lmng::transform const &transform,
  Eigen::Vector3f const &extents,
  std::array<float, 3> colour,
  Eigen::Vector3f const &light_direction)
{
    auto model = get_box_world_matrix(transform, extents);

    lmgl::add_buffer_update(
      frame,
      buffer,
      uniform_data{
        lm::concat(colour, 1.f),
        model,
        camera.view_matrix() * model,
        light_direction,
      });
}

std::tuple<lmgl::buffer, lmgl::buffer, lmgl::buffer, unsigned int>
  create_box_buffers(lmgl::irenderer *renderer)
{
    auto mesh = trimesh::cube(1.f);
    mesh.request_face_normals();
    mesh.update_face_normals();
    std::vector<trimesh::Point> points;
    std::vector<trimesh::Normal> normals;
    std::vector<trimesh::VertexHandle> indices;

    for (auto [i, fh] : lm::enumerate(mesh.faces()))
    {
        auto j = 0;
        // Enumerate not working with fv_range. Bad copy constructor for
        // that iterator?
        for (auto vh : mesh.fv_range(fh))
        {
            points.emplace_back(mesh.point(vh));
            normals.emplace_back(mesh.normal(fh));
            indices.emplace_back(i * 3 + j++);
        }
    }

    return std::tuple{
      renderer->create_buffer(lmgl::buffer_init{
        .usage = lmgl::vertex,
        .data = lm::raw_array_proxy(points),
      }),
      renderer->create_buffer(lmgl::buffer_init{
        .usage = lmgl::vertex,
        .data = lm::raw_array_proxy(normals),
      }),
      renderer->create_buffer(lmgl::buffer_init{
        .usage = lmgl::index,
        .data = lm::raw_array_proxy(indices),
      }),
      indices.size(),
    };
}
} // namespace lmhuv
