#pragma once
#include <Eigen/Eigen>
#include <entt/entt.hpp>

namespace lmng
{
Eigen::Vector3f
  get_extents(entt::registry const &registry, entt::entity entity);
}
