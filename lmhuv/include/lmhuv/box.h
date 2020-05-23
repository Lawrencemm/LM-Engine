#pragma once

#include <tuple>

#include <lmgl/lmgl.h>
#include <lmlib/camera.h>
#include <lmng/shapes.h>
#include <lmng/transform.h>

namespace lmhuv
{
std::tuple<lmgl::buffer, lmgl::buffer, lmgl::buffer, unsigned int>
  create_box_buffers(lmgl::irenderer *renderer);

Eigen::Matrix4f get_box_world_matrix(
  lmng::transform const &transform,
  Eigen::Vector3f const &extents);

Eigen::Matrix4f get_box_mvp_matrix(
  lmng::transform const &transform,
  Eigen::Vector3f const &box_extents,
  lm::camera const &camera);
} // namespace lmhuv
