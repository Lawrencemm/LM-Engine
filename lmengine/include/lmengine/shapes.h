#pragma once

#include <array>

#include <Eigen/Eigen>

namespace lmng
{
struct box_render
{
    Eigen::Vector3f extents;
    std::array<float, 3> colour;
};
} // namespace lmng
