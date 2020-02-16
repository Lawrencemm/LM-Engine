#pragma once

#include "app.h"

#include <lmeditor/map_editor_events.h>

namespace lmeditor
{
bool editor_app::map_editor_handle(lmtk::input_event const &input_event)
{
    map_editor_event_handler event_handler{
      [&](map_editor_changed_selection const &changed_selection_event) {
          resources.inspector->display(
            changed_selection_event.map,
            changed_selection_event.entity,
            resources.resource_sink);
      },
      [&](map_editor_modified_selected const &cleared_selection_event) {
          resources.inspector->update(
            cleared_selection_event.map, resources.resource_sink);
      },
      [&](map_editor_cleared_selection const &cleared_selection_event) {
          resources.inspector->clear(resources.resource_sink);
      },
      [&](map_editor_created_entity const &created_entity_event) {
          sync_entity_list();
      },
      [&](map_editor_destroying_entity const &destroying_entity_event) {
          resources.inspector->clear(resources.resource_sink);
          sync_entity_list();
      },
    };

    return resources.map_editor->handle(
      input_event, resources.resource_sink, event_handler);
}
} // namespace lmeditor
