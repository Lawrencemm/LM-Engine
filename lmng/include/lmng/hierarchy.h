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

class child_iterator
{
  public:
    typedef entt::entity value_type;
    typedef std::ptrdiff_t difference_type;
    typedef entt::entity *pointer;
    typedef entt::entity &reference;
    typedef std::input_iterator_tag iterator_category;

    explicit child_iterator(
      entt::registry const &registry,
      entt::entity parent,
      size_t i)
        : registry{registry}, parent{parent}, i{i}
    {
    }

    entt::entity operator*() const;

    bool operator==(const child_iterator &other) const { return i == other.i; }

    bool operator!=(const child_iterator &other) const
    {
        return !(*this == other);
    }

    child_iterator &operator++()
    {
        i++;
        return *this;
    }

  private:
    entt::registry const &registry;
    entt::entity parent;
    size_t i;
};

class child_range
{
  public:
    child_range(entt::registry const &registry, entt::entity parent)
        : registry{registry}, parent{parent}
    {
    }

    child_iterator begin();
    child_iterator end();

  private:
    entt::registry const &registry;
    entt::entity parent;
};
} // namespace lmng
