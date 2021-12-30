#include "../app.h"
#include <lmlib/variant_visitor.h>

namespace lmeditor
{
lmtk::component_state
  editor_app::modal_state::handle(editor_app &app, lmtk::event const &event)
{
    if (
      event >>
      lm::variant_visitor{
        [&](lmtk::key_down_event const &key_down_event)
        {
            switch (key_down_event.key)
            {
            case lmpl::key_code::Escape:
                app.change_state<gui_state>();
                return true;

            default:
                return false;
            }
        },
        [&](lmtk::quit_event const &)
        {
            modal->move_resources(app.resources.resource_sink);
            return false;
        },
        [](auto) { return false; },
      })
        return lmtk::component_state{0.f};

    return modal->handle(event);
}

void editor_app::modal_state::move_resources(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    modal->move_resources(resource_sink);
}
} // namespace lmeditor
