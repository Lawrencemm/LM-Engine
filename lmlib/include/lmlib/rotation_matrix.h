#pragma once
#include <Eigen/Eigen>

namespace lm
{

inline Eigen::Matrix3f rotation_matrix(const Eigen::Vector3f &euler_angles)
{
    return (Eigen::AngleAxisf{euler_angles.z(), Eigen::Vector3f::UnitZ()} *
            Eigen::AngleAxisf{euler_angles.y(), Eigen::Vector3f::UnitY()} *
            Eigen::AngleAxisf{euler_angles.x(), Eigen::Vector3f::UnitX()})
      .matrix();
}

inline Eigen::Matrix4f rotation_matrix4(const Eigen::Vector3f &euler_angles)
{
    Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
    mat.block<3, 3>(0, 0) = rotation_matrix(euler_angles);
    return mat;
}
} // namespace lm
