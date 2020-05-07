#include "map_editor.h"

#include "../app/app.h"

namespace lmeditor
{
map_editor &map_editor::add_to_frame(lmgl::iframe *frame, editor_app &app)
{
    view.visual_view->update(
      app.map, app.resources.renderer.get(), app.resources.resource_sink);
    view.render(frame, model, app.map);
    return *this;
}
} // namespace lmeditor
