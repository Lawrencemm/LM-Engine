#pragma once

#include "app.h"
#include <lmengine/name.h>

namespace lmeditor
{
bool editor_app::inspector_handle(lmtk::input_event const &input_event)
{
    inspector_event_handler event_handler{
      [&](inspector_updated_data const &updated_data_event) {
          map_editor->update_selection(
            map, updated_data_event.data, updated_data_event.string_repr);
      },
      [&](inspector_added_component const &added_component_event) {
          map_editor->add_component_to_selected(
            map, added_component_event.type, resources.resource_sink);
          inspector->update(map, resources.resource_sink);
      },
      [&](inspector_changed_name const &changed_name_event) {
          map.replace<lmng::name>(
            map_editor->get_selection(),
            lmng::name{changed_name_event.new_name});
          sync_entity_list();
      },
      [&](inspector_removed_component const &removed_component_event) {
          map_editor->remove_component_from_selected(
            map,
            removed_component_event.component_type,
            resources.resource_sink);

          inspector->display(
            map, map_editor->get_selection(), resources.resource_sink);
      }};
    return inspector->handle(
      input_event, resources.resource_sink, event_handler);
}
} // namespace lmeditor
