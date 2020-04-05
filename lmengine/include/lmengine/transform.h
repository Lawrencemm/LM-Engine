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
struct transform_parent
{
    entt::entity entity;
};

void look_at(transform &from, transform const &to);

transform
  resolve_transform(entt::registry const &registry, entt::entity entity);

transform get_frame(entt::registry const &registry, entt::entity entity);

void set_transform_parent(
  entt::registry &registry,
  entt::entity entity,
  entt::entity parent);

template <typename functor_type>
void visit_transform_children(
  entt::registry const &registry,
  entt::entity entity,
  functor_type &&fn)
{
    auto child_view = registry.view<transform_parent const>();

    auto children =
      ranges::views::filter(child_view, [&registry, entity](auto child) {
          return registry.get<transform_parent>(child).entity == entity;
      });

    for (auto child : children)
    {
        fn(child);
    }
}

transform compose_transforms(transform const &lhs, transform const &rhs);
} // namespace lmng
