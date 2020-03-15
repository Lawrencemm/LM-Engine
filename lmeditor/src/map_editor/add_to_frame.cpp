#include "map_editor.h"

#include "../app/app.h"

namespace lmeditor
{
map_editor &map_editor::add_to_frame(lmgl::iframe *frame, editor_app const &app)
{
    view.render(frame, model, app.map);
    return *this;
}
} // namespace lmeditor
