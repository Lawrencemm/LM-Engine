#include "inspector_controller.h"
#include <entt/meta/meta.hpp>
#include <fmt/format.h>
#include <lmeditor/model/selection.h>
#include <lmengine/name.h>
#include <lmengine/reflection.h>
#include <lmlib/enumerate.h>
#include <lmlib/variant_visitor.h>
#include <lmtk/text_editor.h>
#include <range/v3/algorithm/remove.hpp>
#include <range/v3/view/transform.hpp>

namespace lmeditor
{
inspector_controller::inspector_controller(entt::registry &registry)
    : state{empty_state{}},
      registry{&registry},
      entity{entt::null},
      selection_component_created{
        registry.on_construct<selected>()
          .connect<&inspector_controller::on_select>(this)},
      selection_component_destroyed{
        registry.on_destroy<selected>()
          .connect<&inspector_controller::on_deselect>(this)}
{
}

void inspector_controller::inspect_entity(
  entt::registry const &registry,
  entt::entity entity)
{
    clear();
    this->entity = entity;

    create_entries(registry);
    state = select_state{};
}

void inspector_controller::create_entries(const entt::registry &registry)
{
    entries.emplace_back();

    lmng::reflect_components(
      registry, entity, [&](lmng::any_component const &component) {
          entries.emplace_back(entry{component.any.type()});

          component.any.type().data([&](entt::meta_data data) {
              entries.emplace_back(entry{data.parent(), data});
          });
      });
}

void inspector_controller::update(
  entt::registry &registry,
  lmgl::resource_sink &resource_sink)
{
    inspect_entity(registry, entity);
}

void inspector_controller::clear()
{
    entries.clear();
    selected_entry_index = 0;
    state = empty_state{};
    entity = entt::null;
}

bool inspector_controller::handle(lmtk::input_event const &event)
{
    return state >> lm::variant_visitor{[&](auto &state_alternative) {
               return state_alternative.handle(*this, event);
           }};
}

bool inspector_controller::move_selection(int movement)
{
    int new_pos = std::max(
      std::min(selected_entry_index + movement, (int)entries.size() - 1), 0);

    if (selected_entry_index == new_pos)
        return false;

    selected_entry_index = new_pos;
    return true;
}

bool inspector_controller::empty_state::handle(
  lmeditor::inspector_controller &inspector,
  lmtk::input_event const &input_event)
{
    return false;
}

bool inspector_controller::select_state::handle(
  lmeditor::inspector_controller &inspector,
  lmtk::input_event const &input_event)
{
    return input_event >>
           lm::variant_visitor{
             [&](lmtk::key_down_event const &key_down_event) {
                 switch (key_down_event.key)
                 {
                 case lmpl::key_code::I:
                 {
                     return inspector.move_selection(-1);
                 }

                 case lmpl::key_code::K:
                 {
                     return inspector.move_selection(1);
                 }

                 case lmpl::key_code::Enter:
                 {
                     if (inspector.selected_entry_index == 0)
                     {
                         inspector.state.emplace<edit_name_state>(
                           edit_name_state{
                             lmtk::text_editor{lmng::get_name(
                               *inspector.registry, inspector.entity)},
                           });
                     }
                     else
                     {
                         entry &selected_entry = inspector.selected_entry();
                         if (selected_entry.meta_data)
                         {
                             inspector.state.emplace<edit_data_state>(
                               edit_data_state{
                                 lmtk::text_editor{lmng::get_data(
                                   lmng::get_component_any(
                                     *inspector.registry,
                                     inspector.entity,
                                     selected_entry.component_meta_type),
                                   selected_entry.meta_data,
                                   *inspector.registry)},
                               });
                             return true;
                         }
                     }
                     return false;
                 }

                 case lmpl::key_code::X:
                 {
                     entry &selected_entry = inspector.selected_entry();

                     if (selected_entry.meta_data)
                         return false;

                     lmng::remove_from_entity(
                       selected_entry.component_meta_type,
                       *inspector.registry,
                       inspector.entity);
                     return true;
                 }

                 default:
                     return false;
                 }
             },
             [&](auto) { return false; },
           };
}

bool inspector_controller::edit_name_state::handle(
  lmeditor::inspector_controller &inspector,
  lmtk::input_event const &input_event)
{
    return input_event >>
           lm::variant_visitor{
             [&](lmtk::key_down_event const &key_down_msg) {
                 if (text_editor.handle(key_down_msg))
                 {
                     return true;
                 }
                 switch (key_down_msg.key)
                 {
                 case lmpl::key_code::Enter:
                     inspector.registry->replace<lmng::name>(
                       inspector.entity, lmng::name{text_editor.text});
                     inspector.state = select_state{};
                     return true;

                 default:
                     return false;
                 }
             },
             [](auto) { return false; },
           };
}

bool inspector_controller::edit_data_state::handle(
  lmeditor::inspector_controller &inspector,
  lmtk::input_event const &input_event)
{
    return input_event >>
           lm::variant_visitor{
             [&](lmtk::key_down_event const &key_down_msg) {
                 if (text_editor.handle(key_down_msg))
                 {
                     return true;
                 }
                 switch (key_down_msg.key)
                 {
                 case lmpl::key_code::Enter:
                 {
                     auto &entry = inspector.selected_entry();
                     auto component_data = lmng::get_component_any(
                       *inspector.registry,
                       inspector.entity,
                       entry.component_meta_type);
                     lmng::set_data(
                       component_data,
                       entry.meta_data,
                       text_editor.text,
                       *inspector.registry);
                     lmng::replace_on_entity(
                       component_data, *inspector.registry, inspector.entity);
                     inspector.state = select_state{};
                     return true;
                 }

                 default:
                     return false;
                 }
             },
             [](auto) { return false; },
           };
}

std::vector<command_description>
  inspector_controller::get_command_descriptions()
{
    return std::vector<command_description>();
}

void inspector_controller::on_select(
  entt::registry &registry,
  entt::entity entity)
{
    inspect_entity(registry, entity);
}

void inspector_controller::on_deselect(
  entt::registry &registry,
  entt::entity entity)
{
    clear();
}
} // namespace lmeditor
