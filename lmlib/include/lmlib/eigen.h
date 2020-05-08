#pragma once

#include <Eigen/Eigen>

namespace lm
{
inline auto to_array_abs(Eigen::Vector3f const &vec)
{
    return std::array{std::abs(vec[0]), std::abs(vec[1]), std::abs(vec[2])};
}

inline Eigen::Vector3f snap_to_axis(Eigen::Vector3f const &vec)
{
    auto arr = to_array_abs(vec);
    auto max = std::max_element(arr.begin(), arr.end());
    auto max_index = max - arr.begin();
    Eigen::Vector3f ret{0.f, 0.f, 0.f};
    ret[max_index] = std::copysign(1.f, vec[max_index]);
    return ret;
}

inline Eigen::Quaternionf
  pitch_yaw_quat(Eigen::Vector3f const &from, Eigen::Vector3f const &to)
{
    Eigen::Vector3f forward_axis = (to - from).normalized();

    Eigen::Vector3f side_axis{
      Eigen::Vector3f::UnitY().cross(forward_axis).normalized()};

    Eigen::Vector3f up_axis{forward_axis.cross(side_axis)};

    Eigen::Matrix3f rotation_matrix;

    rotation_matrix.col(0) = side_axis;
    rotation_matrix.col(1) = up_axis;
    rotation_matrix.col(2) = forward_axis;

    return Eigen::Quaternionf{rotation_matrix};
}
} // namespace lm
