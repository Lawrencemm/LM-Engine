#pragma once

#include <lmgl/fwd_decl.h>
#include <lmgl/lmgl.h>
#include <lmlib/camera.h>
#include <lmng/shapes.h>
#include <lmng/transform.h>

namespace lmhuv
{
struct box_material_init
{
    lmgl::irenderer &renderer;
    bool write_stencil{false};
    lmgl::polygon_mode polygon_mode{lmgl::polygon_mode::fill};

    lmgl::material operator()();
};

lmgl::buffer create_box_ubuffer(lmgl::irenderer *renderer);

void update_box_uniform(
  lmgl::iframe *frame,
  lmgl::ibuffer *buffer,
  lm::camera const &camera,
  lmng::transform const &transform,
  Eigen::Vector3f const &extents,
  std::array<float, 3> colour,
  Eigen::Vector3f const &light_direction);
} // namespace lmhuv
