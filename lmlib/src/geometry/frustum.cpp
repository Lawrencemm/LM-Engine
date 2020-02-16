#include <cmath>

#include "lmlib/geometry.h"
#include "lmlib/rotation_matrix.h"

namespace lm
{
lm::frustum::frustum(float fov, float aspect, float near, float farClip)
    : nearClip{near}, farClip{farClip}
{
    left = tanf(fov * 0.5f) * near;
    top = left / aspect;
}
Eigen::Matrix4f lm::frustum::perspective_transform()
{
    Eigen::Matrix4f mat;
    mat << nearClip / left, 0.f, 0.f, 0.f, 0.f, -nearClip / top, 0.f, 0.f,
      perspective_z_transform().transpose(), 0.f, 0.f, 1.f, 0.f;
    return mat;
}

Eigen::Vector4f frustum::perspective_z_transform()
{
    const float far_minus_near = farClip - nearClip;
    return {
      0.f, 0.f, farClip / far_minus_near, -farClip * nearClip / far_minus_near};
}
} // namespace lm