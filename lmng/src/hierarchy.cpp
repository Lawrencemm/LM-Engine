#include <entt/entt.hpp>
#include <lmng/hierarchy.h>
#include <lmng/name.h>
#include <lmng/transform.h>
#include <range/v3/action/remove.hpp>
#include <spdlog/spdlog.h>

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
    auto p_children = registry.try_get<internal::children>(parent);
    if (p_children)
        return p_children->entities.begin();

    return child_iterator{};
}

child_iterator child_range::end()
{
    auto p_children = registry.try_get<internal::children>(parent);
    if (p_children)
        return p_children->entities.end();

    return child_iterator{};
}

child_range::child_range(const entt::registry &registry, entt::entity parent)
    : registry{registry}, parent{parent}
{
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

entt::entity find_child(
  entt::registry &registry,
  entt::entity parent,
  std::string const &name)
{
    entt::entity result{entt::null};

    for (auto child : child_range{registry, parent})
    {
        if (registry.get<lmng::name>(child).string == name)
        {
            result = child;
            break;
        }

        auto child_result = find_child(registry, child, name);
        if (child_result != entt::null)
        {
            result = child_result;
            break;
        }
    }

    return result;
}

recursive_child_iterator::recursive_child_iterator(
  const entt::registry &registry,
  entt::entity root)
    : registry{registry}
{
    if (root == entt::null)
        return;

    auto p_parent_children = registry.try_get<internal::children>(root);

    if (p_parent_children)
    {
        child_range_stack.emplace_back(std::pair{
          p_parent_children->entities.begin(),
          p_parent_children->entities.end()});
    }
}

recursive_child_iterator &recursive_child_iterator::operator++()
{
    SPDLOG_DEBUG(
      "Recursive child iterator: Increment with current entity {}",
      lmng::get_name(registry, *child_range_stack.back().first));

    auto &back = child_range_stack.back();

    auto maybe_children = registry.try_get<internal::children>(*back.first);
    if (maybe_children && !maybe_children->entities.empty())
    {
        SPDLOG_DEBUG(
          "Recursive child iterator: entity {} has children, appending child "
          "range to stack",
          lmng::get_name(registry, *child_range_stack.back().first));

        child_range_stack.emplace_back(std::pair{
          maybe_children->entities.begin(), maybe_children->entities.end()});

        SPDLOG_DEBUG(
          "Recursive child iterator: Moved to next ({})",
          lmng::get_name(registry, *child_range_stack.back().first));

        return *this;
    }

    SPDLOG_DEBUG(
      "Recursive child iterator: entity {} does not have children; moving to "
      "next",
      lmng::get_name(registry, *child_range_stack.back().first));

    while (child_range_stack.back().first + 1 ==
           child_range_stack.back().second)
    {
        SPDLOG_DEBUG(
          "Recursive child iterator: Popping top child range at "
          "end ({})",
          lmng::get_name(registry, *child_range_stack.back().first));

        child_range_stack.pop_back();

        if (child_range_stack.empty())
        {
            SPDLOG_DEBUG("Recursive child iterator: Stack empty");
            return *this;
        }
    }

    child_range_stack.back().first++;
    SPDLOG_DEBUG(
      "Recursive child iterator: Moved to next ({})",
      lmng::get_name(registry, *child_range_stack.back().first));

    return *this;
}

bool recursive_child_iterator::operator==(
  const recursive_child_iterator &other) const
{
    if (other.child_range_stack.empty())
        return child_range_stack.empty();

    return child_range_stack.back().first ==
           other.child_range_stack.back().first;
}

bool recursive_child_iterator::operator!=(
  const recursive_child_iterator &other) const
{
    return !(*this == other);
}

entt::entity recursive_child_iterator::operator*() const
{
    return *child_range_stack.back().first;
}

recursive_child_range::recursive_child_range(
  const entt::registry &registry,
  entt::entity root)
    : registry{registry}, root{root}
{
}

recursive_child_iterator recursive_child_range::begin()
{
    return recursive_child_iterator(registry, root);
}

recursive_child_iterator recursive_child_range::end()
{
    return recursive_child_iterator(registry, entt::null);
}
} // namespace lmng
