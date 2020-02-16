#include "app.h"

namespace lmeditor
{
bool editor_app::entity_list_handle(lmtk::input_event const &input_event)
{
    bool handled =
      input_event >>
      lm::variant_visitor{
        [&](lmtk::key_down_event const &key_down_event) {
            switch (key_down_event.key)
            {
            case lmpl::key_code::Enter:
            {
                entt::entity new_selection{resources.entity_list->get_selection(
                  resources.map_editor->get_map())};
                resources.map_editor->select(new_selection);
                resources.inspector->display(
                  resources.map_editor->get_map(),
                  new_selection,
                  resources.resource_sink);
                return true;
            }

            default:
                return false;
            }
        },
        [](auto &) { return false; },
      };
    if (handled)
        return true;

    return resources.entity_list->handle(input_event);
}
} // namespace lmeditor
