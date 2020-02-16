#include <lmlib/camera.h>
#include <lmlib/geometry.h>
#include <lmlib/rotation_matrix.h>

namespace lm
{
std::pair<Eigen::Vector3f, Eigen::Vector3f>
  camera::ray_from_point(float x, float y)
{
    auto frustum_ = frustum{fov, aspect, near_clip, far_clip};
    Eigen::Matrix4f inverse_matrix =
      (frustum_.perspective_transform() * inverse_transform_matrix()).inverse();
    float from_z = frustum_.perspective_z_transform().transpose() *
                   Eigen::Vector4f{0.f, 0.f, frustum_.nearClip, 1.f};
    auto from_ndc = Eigen::Vector4f{
      x * frustum_.nearClip, y * frustum_.nearClip, from_z, frustum_.nearClip};
    Eigen::Vector4f from_point = inverse_matrix * from_ndc;
    float to_z = frustum_.perspective_z_transform().transpose() *
                 Eigen::Vector4f{0.f, 0.f, frustum_.farClip, 1.f};
    auto to_ndc = Eigen::Vector4f{
      x * frustum_.farClip, y * frustum_.farClip, to_z, frustum_.farClip};
    Eigen::Vector4f to_point = inverse_matrix * to_ndc;
    return {from_point.head(3), to_point.head(3)};
}

Eigen::Matrix4f camera::transform_matrix() const
{
    return Eigen::Affine3f{Eigen::Translation3f{pos} * rotation}.matrix();
}

Eigen::Matrix4f camera::inverse_transform_matrix() const
{
    Eigen::Matrix4f matrix = transform_matrix();
    return matrix.inverse();
}

Eigen::Matrix4f camera::view_matrix() const
{
    return projection_matrix() * inverse_transform_matrix();
}

Eigen::Matrix4f camera::projection_matrix() const
{
    auto frustum_ = frustum{fov, aspect, near_clip, far_clip};
    return frustum_.perspective_transform();
}

Eigen::Vector4f camera::get_ndc(const Eigen::Vector3f &point) const
{
    Eigen::Vector4f homogeneous_coord;
    homogeneous_coord << point, 1.f;
    Eigen::Vector4f clip_coord = view_matrix() * homogeneous_coord;
    Eigen::Vector4f ndc_coord;
    ndc_coord << clip_coord.head(3) / clip_coord[3], clip_coord[3];
    return ndc_coord;
}

Eigen::Vector3f camera::dir() const
{
    return rotation * Eigen::Vector3f(0.f, 0.f, -1.f);
}
} // namespace lm
