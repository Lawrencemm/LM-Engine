#pragma once

#include <entt/fwd.hpp>
#include <lmeditor/model/command.h>
#include <lmng/name.h>
#include <lmtk/event.h>

namespace lmeditor
{
struct entity_list_init;

class entity_list_controller
{
  public:
    explicit entity_list_controller(entt::registry &registry);

    std::vector<command_description> get_command_descriptions();

    entt::entity get_selection(entt::registry const &registry);

    bool handle(lmtk::event const &event);

    entity_list_controller &display(entt::registry const &registry);

    void reset(const entt::registry &registry);

    void on_name_contructed(entt::registry &registry, entt::entity entity);

    void on_name_destroyed(entt::registry &registry, entt::entity entity);

    void on_name_replaced(entt::registry &registry, entt::entity entity);

    size_t size();

    entt::registry *registry;
    entt::scoped_connection name_constructed_connection,
      name_destroyed_connection, name_replaced_connection;
    int selected_entity_index, named_entities_count;
    std::vector<entt::entity> entities;
    bool dirty{false};

    bool handle_key_down(lmtk::key_down_event const &event);
    bool move_selection(int movement);
};
} // namespace lmeditor
