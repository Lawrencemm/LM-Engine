#pragma once

#include <fmt/format.h>
#include <ostream>

struct protagonist_component
{
    float move_force{10.f}, jump_impulse{10.f};
};

inline std::ostream &
  operator<<(std::ostream &os, protagonist_component const &protagonist)
{
    os << fmt::format(
      "Move force: {} Jump impulse: {}",
      protagonist.move_force,
      protagonist.jump_impulse);
    return os;
}
