#pragma once

#include <entt/entt.hpp>
#include <lmlib/geometry.h>
#include <range/v3/view/filter.hpp>

namespace lmng
{
struct transform
{
    Eigen::Vector3f position{Eigen::Vector3f::Zero()};
    Eigen::Quaternionf rotation{Eigen::Quaternionf::Identity()};
};
inline bool operator==(transform const &lhs, transform const &rhs)
{
    return lhs.position == rhs.position &&
           lhs.rotation.vec() == rhs.rotation.vec();
}

void look_at(transform &from, transform const &to);

transform
  resolve_transform(entt::registry const &registry, entt::entity entity);

transform get_frame(entt::registry const &registry, entt::entity entity);

transform compose_transforms(transform const &lhs, transform const &rhs);
} // namespace lmng
