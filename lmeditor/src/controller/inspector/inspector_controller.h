#pragma once

#include <entt/entity/registry.hpp>
#include <entt/meta/meta.hpp>
#include <lmeditor/component/inspector.h>
#include <lmeditor/model/command.h>
#include <lmtk/event.h>
#include <lmtk/text_editor.h>

namespace lmeditor
{
class inspector_controller
{
  public:
    explicit inspector_controller(entt::registry &registry);

    bool handle(lmtk::event const &event);

    void update(entt::registry &registry, lmgl::resource_sink &resource_sink);

    void inspect_entity(entt::registry const &registry, entt::entity entity);

    void clear();

    std::vector<command_description> get_command_descriptions();

    bool move_selection(int movement);
    void create_entries(entt::registry const &registry);

    void on_select(entt::registry &registry, entt::entity entity);
    void on_deselect(entt::registry &registry, entt::entity entity);

    struct empty_state
    {
        bool handle(
          lmeditor::inspector_controller &inspector,
          lmtk::event const &event);
    };
    struct select_state
    {
        bool handle(
          lmeditor::inspector_controller &inspector,
          lmtk::event const &event);
    };
    struct edit_name_state
    {
        lmtk::text_editor text_editor;
        bool handle(
          lmeditor::inspector_controller &inspector,
          lmtk::event const &event);
    };
    struct edit_data_state
    {
        lmtk::text_editor text_editor;
        bool handle(
          lmeditor::inspector_controller &inspector,
          lmtk::event const &event);
    };

    using state_variant_type =
      std::variant<empty_state, select_state, edit_name_state, edit_data_state>;

    state_variant_type state;

    entt::registry *registry;
    entt::entity entity;

    entt::scoped_connection selection_component_created,
      selection_component_destroyed;

    struct entry
    {
        entt::meta_type component_meta_type;
        entt::meta_data meta_data;
    };
    std::vector<entry> entries;

    int selected_entry_index{0};

    entry &selected_entry() { return entries[selected_entry_index]; }
};
} // namespace lmeditor
