#pragma once

#include "app.h"
#include <lmengine/name.h>
#include <lmengine/reflection.h>

namespace lmeditor
{
bool editor_app::inspector_handle(lmtk::input_event const &input_event)
{
    inspector_event_handler event_handler{
      [&](inspector_updated_data const &updated_data_event) {
          auto component = lmng::any_component{
            map, map_editor->get_selection(), updated_data_event.data.parent()};
          component.set(
            updated_data_event.data, updated_data_event.string_repr, map);
          component.replace(map, map_editor->get_selection());
      },
      [&](inspector_added_component const &added_component_event) {
          auto component = added_component_event.type.ctor().invoke();
          lmng::assign_to_entity(component, map, map_editor->get_selection());
          inspector->update(map, resources.resource_sink);
      },
      [&](inspector_changed_name const &changed_name_event) {
          map.replace<lmng::name>(
            map_editor->get_selection(),
            lmng::name{changed_name_event.new_name});
          sync_entity_list();
      },
      [&](inspector_removed_component const &removed_component_event) {
          lmng::remove_from_entity(
            removed_component_event.component_type,
            map,
            map_editor->get_selection());
          inspector->display(
            map, map_editor->get_selection(), resources.resource_sink);
      }};
    return inspector->handle(
      input_event, resources.resource_sink, event_handler);
}
} // namespace lmeditor
