#pragma once
#include <Eigen/Eigen>

namespace lmng
{
struct kinematic_rotation
{
    float angle;
    Eigen::Vector3f axis;
};
}
