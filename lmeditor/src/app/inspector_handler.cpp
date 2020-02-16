#pragma once

#include "app.h"
#include <lmengine/name.h>

namespace lmeditor
{
bool editor_app::inspector_handle(lmtk::input_event const &input_event)
{
    inspector_event_handler event_handler{
      [&](inspector_updated_data const &updated_data_event) {
          resources.map_editor->update_selection(
            updated_data_event.data, updated_data_event.string_repr);
      },
      [&](inspector_added_component const &added_component_event) {
          resources.map_editor->add_component_to_selected(
            added_component_event.type, resources.resource_sink);
          resources.inspector->update(
            resources.map_editor->get_map(), resources.resource_sink);
      },
      [&](inspector_changed_name const &changed_name_event) {
          resources.map_editor->get_map().replace<lmng::name>(
            resources.map_editor->get_selection(),
            lmng::name{changed_name_event.new_name});
          sync_entity_list();
      },
      [&](inspector_removed_component const &removed_component_event) {
          resources.map_editor->remove_component_from_selected(
            removed_component_event.component_type, resources.resource_sink);

          resources.inspector->display(
            resources.map_editor->get_map(),
            resources.map_editor->get_selection(),
            resources.resource_sink);
      }};
    return resources.inspector->handle(
      input_event, resources.resource_sink, event_handler);
}
} // namespace lmeditor
