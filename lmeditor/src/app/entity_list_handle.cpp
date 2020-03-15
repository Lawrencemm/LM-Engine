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
                entt::entity new_selection{entity_list->get_selection(map)};
                map_editor->select(new_selection);
                inspector->display(map, new_selection, resources.resource_sink);
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

    return entity_list->handle(input_event);
}
} // namespace lmeditor
