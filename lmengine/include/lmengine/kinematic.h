#pragma once
#include <Eigen/Eigen>

#include "named_type.h"

namespace lmng
{
struct kinematic_rotation
{
    float angle;
    Eigen::Vector3f axis;
};
}

LMNG_NAMED_TYPE(lmng::kinematic_rotation, "Kinematic Rotation")
