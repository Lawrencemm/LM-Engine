#pragma once

#include <entt/entt.hpp>

namespace lmeditor
{
struct map_editor_changed_selection
{
    entt::registry &map;
    entt::entity entity;
};
struct map_editor_modified_selected
{
    entt::registry &map;
};
struct map_editor_cleared_selection
{
};
struct map_editor_created_entity
{
    entt::registry &map;
    entt::entity entity;
};
struct map_editor_destroying_entity
{
    entt::registry &map;
    entt::entity entity;
};

struct map_editor_event_handler
{
    // Use verb_noun naming convention for events.
    using changed_selection_handler =
      std::function<void(map_editor_changed_selection const &)>;
    using modified_selection_handler =
      std::function<void(map_editor_modified_selected const &)>;
    using cleared_selection_handler =
      std::function<void(map_editor_cleared_selection const &)>;
    using created_entity_handler =
      std::function<void(map_editor_created_entity const &)>;
    using destroying_entity_handler =
      std::function<void(map_editor_destroying_entity const &)>;

    changed_selection_handler on_changed_selection{[](auto &) {}};
    modified_selection_handler on_modified_selected{[](auto &) {}};
    cleared_selection_handler on_cleared_selection{[](auto &) {}};
    created_entity_handler on_created_entity{[](auto &) {}};
    destroying_entity_handler on_destroying_entity{[](auto &) {}};

    map_editor_event_handler &set_on_created_entity(created_entity_handler &&eh)
    {
        on_created_entity = eh;
        return *this;
    }
    map_editor_event_handler &
      set_on_destroying_entity(destroying_entity_handler &&eh)
    {
        on_destroying_entity = eh;
        return *this;
    }

    void operator()(map_editor_changed_selection const &event) const
    {
        on_changed_selection(event);
    }
    void operator()(map_editor_modified_selected const &event) const
    {
        on_modified_selected(event);
    }
    void operator()(map_editor_cleared_selection const &event) const
    {
        on_cleared_selection(event);
    }
    void operator()(map_editor_created_entity const &event) const
    {
        on_created_entity(event);
    }
    void operator()(map_editor_destroying_entity const &event) const
    {
        on_destroying_entity(event);
    }

    map_editor_event_handler copy() const { return *this; }
};
} // namespace lmeditor
