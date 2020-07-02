#pragma once

#include <entt/entt.hpp>
#include <entt/signal/sigh.hpp>
#include <iterator>

namespace lmng
{
struct parent
{
    entt::entity entity;
};

class hierarchy_system
{
  public:
    explicit hierarchy_system(entt::registry &registry);

  private:
    entt::scoped_connection parent_construct, parent_destroy, children_destroy;
};

void reparent(
  entt::registry &registry,
  entt::entity child,
  entt::entity new_parent);

void orphan_children(entt::registry &registry, entt::entity parent);

entt::entity find_child(
  entt::registry &registry,
  entt::entity parent,
  std::string const &name);

using child_iterator = std::vector<entt::entity>::const_iterator;

class child_range
{
  public:
    child_range(entt::registry const &registry, entt::entity parent);

    child_iterator begin();
    child_iterator end();

  private:
    entt::registry const &registry;
    entt::entity parent;
};
} // namespace lmng
