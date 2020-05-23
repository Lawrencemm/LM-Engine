#include "../app.h"
#include <lmlib/variant_visitor.h>

namespace lmeditor
{
bool editor_app::modal_state::handle(
  editor_app &app,
  lmtk::input_event const &input_event)
{
    if (
      input_event >>
      lm::variant_visitor{
        [&](lmtk::key_down_event const &key_down_event) {
            switch (key_down_event.key)
            {
            case lmpl::key_code::Escape:
                app.change_state<gui_state>();
                return true;

            default:
                return false;
            }
        },
        [](auto) { return false; },
      })
        return true;

    return modal->handle(input_event);
}

void editor_app::modal_state::move_resources(
  lmgl::irenderer *renderer,
  lmgl::resource_sink &resource_sink)
{
    modal->move_resources(resource_sink);
}

bool editor_app::modal_state::add_to_frame(editor_app &app, lmgl::iframe *frame)
{
    modal->update(
      app.resources.renderer.get(),
      app.resources.resource_sink,
      app.resource_cache,
      app.resources.input_state);

    return modal->add_to_frame(frame);
}
} // namespace lmeditor
