#include <entt/entt.hpp>
#include <lmng/hierarchy.h>
#include <lmng/transform.h>
#include <range/v3/action/remove.hpp>

namespace lmng
{
namespace internal
{
struct children
{
    std::vector<entt::entity> entities;
};
} // namespace internal

void on_parent_constructed(entt::registry &registry, entt::entity entity)
{
    entt::entity parent = registry.get<lmng::parent>(entity).entity;

    auto p_children = registry.try_get<internal::children>(parent);

    if (p_children)
    {
        p_children->entities.emplace_back(entity);
    }
    else
    {
        auto &children = registry.assign<internal::children>(parent);
        children.entities.emplace_back(entity);
    }
}

void on_parent_removed(entt::registry &registry, entt::entity entity)
{
    entt::entity parent = registry.get<lmng::parent>(entity).entity;
    auto &children = registry.get<lmng::internal::children>(parent);
    ranges::actions::remove(children.entities, entity);
}

void on_children_remove(entt::registry &registry, entt::entity parent)
{
    for (auto child : child_range(registry, parent))
    {
        registry.remove<lmng::parent>(child);
    }
}

hierarchy_system::hierarchy_system(entt::registry &registry)
    : parent_construct{registry.on_construct<parent>()
                         .connect<&on_parent_constructed>(),},
      parent_destroy{
        registry.on_destroy<parent>().connect<&on_parent_removed>(),},
      children_destroy{registry.on_destroy<internal::children>()
                         .connect<&on_children_remove>(),}
{
    for (auto entity : registry.view<parent>())
    {
        on_parent_constructed(registry, entity);
    }
}

child_iterator child_range::begin()
{
    return child_iterator(registry, parent, 0);
}

child_iterator child_range::end()
{
    auto p_children = registry.try_get<internal::children>(parent);
    return child_iterator(
      registry, parent, p_children ? p_children->entities.size() : 0);
}

entt::entity child_iterator::operator*() const
{
    return registry.get<internal::children>(parent).entities[i];
}

void reparent(
  entt::registry &registry,
  entt::entity child,
  entt::entity new_parent)
{
    if (registry.try_get<transform>(child))
    {
        auto parent_transform = lmng::resolve_transform(registry, new_parent);
        auto child_transform = lmng::resolve_transform(registry, child);

        auto diff_transform = lmng::transform{
          parent_transform.rotation.inverse() *
            (child_transform.position - parent_transform.position),
          parent_transform.rotation.inverse() * child_transform.rotation,
        };

        Eigen::Vector3f diff_forward =
          diff_transform.rotation * Eigen::Vector3f::UnitZ();

        registry.replace<lmng::transform>(child, diff_transform);
    }

    registry.remove_if_exists<parent>(child);
    registry.assign<parent>(child, new_parent);
}

void orphan_children(entt::registry &registry, entt::entity parent)
{
    auto child_range = lmng::child_range{registry, parent};

    for (auto child : child_range)
    {
        if (auto p_child_transform = registry.try_get<transform>(child))
        {
            registry.replace<transform>(
              child, resolve_transform(registry, child));
        }
    }

    std::vector<entt::entity> copied_children{
      child_range.begin(), child_range.end()};
    registry.remove<lmng::parent>(
      copied_children.begin(), copied_children.end());

    registry.remove<internal::children>(parent);
}
} // namespace lmng
