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

class recursive_child_iterator
{
  public:
    using value_type = entt::entity;
    using difference_type = std::ptrdiff_t;
    using pointer = entt::entity *;
    using reference = entt::entity &;
    using iterator_category = std::input_iterator_tag;

    recursive_child_iterator(entt::registry const &registry, entt::entity root);

    entt::entity operator*() const;

    bool operator==(const recursive_child_iterator &other) const;

    bool operator!=(const recursive_child_iterator &other) const;

    recursive_child_iterator &operator++();

  private:
    entt::registry const &registry;
    std::vector<std::pair<child_iterator, child_iterator>> child_range_stack;
};

class recursive_child_range
{
  public:
    recursive_child_range(entt::registry const &registry, entt::entity root);

    recursive_child_iterator begin();
    recursive_child_iterator end();

  private:
    entt::registry const &registry;
    entt::entity root;
};
} // namespace lmng
