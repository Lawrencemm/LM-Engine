#include "entity_list_controller.h"
#include <entt/entt.hpp>
#include <entt/fwd.hpp>
#include <lmeditor/model/command.h>
#include <lmeditor/model/selection.h>
#include <lmgl/lmgl.h>
#include <lmlib/variant_visitor.h>
#include <lmng/hierarchy.h>
#include <lmng/name.h>
#include <lmtk/component.h>
#include <lmtk/font.h>
#include <lmtk/event.h>

namespace lmeditor
{
entity_list_controller::entity_list_controller(entt::registry &registry)
    : registry{&registry},
      name_constructed_connection{
        registry.on_construct<lmng::name>()
          .connect<&entity_list_controller::on_name_contructed>(this)},
      name_destroyed_connection{
        registry.on_destroy<lmng::name>()
          .connect<&entity_list_controller::on_name_destroyed>(this)},
      name_replaced_connection{
        registry.on_update<lmng::name>()
          .connect<&entity_list_controller::on_name_replaced>(this)},
      selected_entity_index{0},
      named_entities_count{0}
{
}

bool entity_list_controller::handle(lmtk::event const &event)
{
    return event >> lm::variant_visitor{
                      [&](lmtk::key_down_event const &key_down_event)
                      { return handle_key_down(key_down_event); },
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
        unsigned iterated{0};
        for (auto parent :
             registry->view<lmng::name>(entt::exclude<lmng::parent>))
        {
            if (iterated == selected_entity_index)
            {
                lmeditor::select(*registry, parent);
                return true;
            }
            iterated++;
            for (auto child : lmng::recursive_child_range{*registry, parent})
            {
                if (iterated == selected_entity_index)
                {
                    lmeditor::select(*registry, child);
                    return true;
                }
                iterated++;
            }
        }
        return true;
    }

    default:
        return false;
    }
}

bool entity_list_controller::move_selection(int movement)
{
    int new_pos = std::max(
      std::min(selected_entity_index + movement, named_entities_count - 1), 0);

    if (selected_entity_index == new_pos)
        return false;

    selected_entity_index = new_pos;

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

void entity_list_controller::on_name_destroyed(
  entt::registry &registry,
  entt::entity entity)
{
    dirty = true;
    named_entities_count--;
    if (
      named_entities_count > 0 && selected_entity_index >= named_entities_count)
    {
        selected_entity_index = named_entities_count - 1;
    }
    else
    {
        selected_entity_index = 0;
    }
}

void entity_list_controller::on_name_contructed(
  entt::registry &registry,
  entt::entity entity)
{
    dirty = true;
    named_entities_count++;
}

void entity_list_controller::on_name_replaced(
  entt::registry &registry,
  entt::entity entity)
{
    dirty = true;
}
} // namespace lmeditor
