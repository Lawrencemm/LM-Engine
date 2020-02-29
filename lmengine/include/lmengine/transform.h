#pragma once

#include <entt/entity/fwd.hpp>

#include <lmlib/geometry.h>

namespace lmng
{
struct transform
{
    Eigen::Vector3f position{Eigen::Vector3f::Zero()};
    Eigen::Quaternionf rotation{Eigen::Quaternionf::Identity()};
};
struct transform_parent
{
    entt::entity entity;
};

void look_at(transform &from, transform const &to);

transform
  resolve_transform(entt::registry const &registry, entt::entity entity);

transform get_frame(entt::registry const &registry, entt::entity entity);
} // namespace lmng
