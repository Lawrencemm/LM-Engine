#include "entity_list_controller.h"
#include <entt/entt.hpp>
#include <lmeditor/model/command.h>
#include <lmeditor/model/selection.h>
#include <lmlib/variant_visitor.h>
#include <lmng/name.h>
#include <lmtk/input_event.h>
#include <spdlog/spdlog.h>

namespace lmeditor
{
entity_list_controller::entity_list_controller(entt::registry &registry)
    : registry{&registry},
      name_constructed_connection{
        registry.on_construct<lmng::name>()
          .connect<&entity_list_controller::on_name_constructed>(this)},
      name_destroyed_connection{
        registry.on_destroy<lmng::name>()
          .connect<&entity_list_controller::on_name_destroyed>(this)},
      name_replaced_connection{
        registry.on_replace<lmng::name>()
          .connect<&entity_list_controller::on_name_replaced>(this)},
      selected_entity_index{0}
{
}

bool entity_list_controller::handle(lmtk::input_event const &input_event)
{
    return input_event >> lm::variant_visitor{
                            [&](lmtk::key_down_event const &key_down_event) {
                                return handle_key_down(key_down_event);
                            },
                            [](auto) { return false; },
                          };
}

bool entity_list_controller::handle_key_down(lmtk::key_down_event const &event)
{
    switch (event.key)
    {
    case lmpl::key_code::I:
        return move_selection(-1);

    case lmpl::key_code::K:
        return move_selection(1);

    case lmpl::key_code::Enter:
    {
        if (!registry->view<lmng::name>().empty())
        {
            lmeditor::select(
              *registry, registry->view<lmng::name>()[selected_entity_index]);
        }
        return true;
    }

    default:
        return false;
    }
}

bool entity_list_controller::move_selection(int movement)
{
    auto old_index = selected_entity_index;

    auto name_view = registry->view<lmng::name>();

    int new_pos = std::max(
      std::min(selected_entity_index + movement, (int)name_view.size() - 1), 0);

    if (selected_entity_index == new_pos)
        return false;

    selected_entity_index = new_pos;

    SPDLOG_INFO(
      "Entity list: Move selection {} from {} to {}",
      movement == 1 ? "down" : "up",
      lmng::get_name(*registry, name_view[old_index]),
      lmng::get_name(*registry, name_view[selected_entity_index]));

    return true;
}

std::vector<command_description>
  entity_list_controller::get_command_descriptions()
{
    return std::vector<command_description>();
}

entity_list_controller &
  entity_list_controller::display(entt::registry const &registry)
{
    selected_entity_index = 0;
    return *this;
}

entt::entity
  entity_list_controller::get_selection(entt::registry const &registry)
{
    return registry.view<lmng::name const>()[selected_entity_index];
}

void entity_list_controller::reset(const entt::registry &registry) {}

size_t entity_list_controller::size() { return 0; }

void entity_list_controller::on_name_constructed(
  entt::registry &registry,
  entt::entity entity)
{
    dirty = true;
    SPDLOG_INFO(
      "Entity list: adding entity {}", registry.get<lmng::name>(entity).string);
}

void entity_list_controller::on_name_destroyed(
  entt::registry &registry,
  entt::entity entity)
{
    dirty = true;
    SPDLOG_INFO(
      "Entity list: removing entity {}",
      registry.get<lmng::name>(entity).string);

    auto name_view = registry.view<lmng::name>();

    int remaining = name_view.size() - 1;
    selected_entity_index = std::min(selected_entity_index, remaining - 1);
}

void entity_list_controller::on_name_replaced(
  entt::registry &registry,
  entt::entity entity)
{
    dirty = true;
}
} // namespace lmeditor
