#pragma once

#include <array>

#include "named_type.h"
#include <Eigen/Eigen>

namespace lmng
{
struct box_render
{
    Eigen::Vector3f extents;
    std::array<float, 3> colour;
};
} // namespace lmng

LMNG_NAMED_TYPE(lmng::box_render, "Box Render");
