#pragma once

#include <Eigen/Eigen>
#include <array>
#include <fmt/format.h>

namespace lmng
{
struct box_render
{
    Eigen::Vector3f extents;
    std::array<float, 3> colour;
};
} // namespace lmng

inline std::ostream &operator<<(std::ostream &os, lmng::box_render const &box_render)
{
    os << fmt::format(
      "X: {} Y: {} Z: {} R: {} G: {} B: {}",
      box_render.extents[0],
      box_render.extents[1],
      box_render.extents[2],
      box_render.colour[0],
      box_render.colour[1],
      box_render.colour[2]);
    return os;
}
