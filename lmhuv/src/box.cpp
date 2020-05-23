#include <lmhuv/box.h>
#include <lmlib/camera.h>
#include <lmng/shapes.h>
#include <lmng/transform.h>

namespace lmhuv
{
Eigen::Matrix4f get_box_world_matrix(
  lmng::transform const &transform,
  Eigen::Vector3f const &extents)
{
    return (Eigen::Translation3f{transform.position} *
            Eigen::Affine3f(transform.rotation) * Eigen::Scaling(extents))
      .matrix();
}

Eigen::Matrix4f get_box_mvp_matrix(
  lmng::transform const &transform,
  Eigen::Vector3f const &box_extents,
  lm::camera const &camera)
{
    return camera.view_matrix() *
           (Eigen::Translation3f{transform.position} *
            Eigen::Affine3f(transform.rotation) * Eigen::Scaling(box_extents))
             .matrix();
}
} // namespace lmhuv
