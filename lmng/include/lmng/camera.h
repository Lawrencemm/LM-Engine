#pragma once

#include <fmt/format.h>

namespace lmng
{
struct camera
{
    float field_of_view, near_clip, far_clip;
    bool active;
};
} // namespace lmng

inline std::ostream &operator<<(std::ostream &os, lmng::camera const &camera)
{
    os << fmt::format(
      "FOV: {} Near: {} Far: {} Active: {}",
      camera.field_of_view,
      camera.near_clip,
      camera.far_clip,
      camera.active);
    return os;
}
